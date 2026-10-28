/*
 * Video inspection geometry for MPC-HC.
 * Copyright (C) 2026. Licensed under GPL v3 or later; see COPYING.txt.
 */
#pragma once
#include <algorithm>
#include <cmath>

namespace VideoInspection {
struct Rect {
    double left, top, right, bottom;
    double width() const { return right - left; }
    double height() const { return bottom - top; }
};

class Transform {
    double m_zoom = 1.0;
    // Offsets in units of the uninspected rectangle, so resize/fullscreen scales them.
    double m_offsetX = 0.0, m_offsetY = 0.0;
    static constexpr double MaxCoordinate = 10000000.0;

    static bool Coordinate(double value) {
        return std::isfinite(value) && std::abs(value) <= MaxCoordinate;
    }
    static bool Valid(Rect r) {
        return Coordinate(r.left) && Coordinate(r.top) && Coordinate(r.right)
            && Coordinate(r.bottom) && r.width() > 0.0 && r.height() > 0.0;
    }
    static double Constrain(double origin, double extent, double start, double end) {
        return extent <= end - start ? start + (end - start - extent) * 0.5
                                    : std::clamp(origin, end - extent, start);
    }
    static Rect Constrain(Rect r, Rect viewport) {
        const double width = r.width(), height = r.height();
        const double x = Constrain(r.left, width, viewport.left, viewport.right);
        const double y = Constrain(r.top, height, viewport.top, viewport.bottom);
        return {x, y, x + width, y + height};
    }
    void Store(Rect r, Rect base) {
        m_offsetX = (r.left - base.left) / base.width();
        m_offsetY = (r.top - base.top) / base.height();
    }

public:
    bool Active() const { return m_zoom > 1.0; }
    double Zoom() const { return m_zoom; }
    void Reset() { m_zoom = 1.0; m_offsetX = m_offsetY = 0.0; }
    Rect Apply(Rect base, Rect viewport) const {
        if (!Active() || !Valid(base) || !Valid(viewport)) {
            return base;
        }
        const double x = base.left + m_offsetX * base.width();
        const double y = base.top + m_offsetY * base.height();
        return Constrain({x, y, x + base.width() * m_zoom, y + base.height() * m_zoom}, viewport);
    }
    bool ZoomAt(double delta, double x, double y, Rect base, Rect viewport) {
        if (!std::isfinite(delta) || delta == 0.0 || !Coordinate(x) || !Coordinate(y)
                || !Valid(base) || !Valid(viewport)) {
            return false;
        }
        // Limit the exponent before pow, including extreme but finite input deltas.
        const double factor = std::pow(1.1, std::clamp(delta / 120.0, -32.0, 32.0));
        double zoom = std::clamp(m_zoom * factor, 1.0, 8.0);
        if (zoom < 1.0 + 1e-12) {
            zoom = 1.0;
        }
        if (zoom == m_zoom) {
            return false;
        }
        if (zoom == 1.0) {
            Reset();
            return true;
        }
        const Rect old = Apply(base, viewport);
        const double ratio = zoom / m_zoom;
        const double left = x - (x - old.left) * ratio;
        const double top = y - (y - old.top) * ratio;
        const Rect next{left, top, left + base.width() * zoom, top + base.height() * zoom};
        m_zoom = zoom;
        Store(Constrain(next, viewport), base);
        return true;
    }
    void Pan(double dx, double dy, Rect base, Rect viewport) {
        if (!Active() || !std::isfinite(dx) || !std::isfinite(dy) || !Valid(base) || !Valid(viewport)) {
            return;
        }
        Rect r = Apply(base, viewport);
        dx = std::clamp(dx, -MaxCoordinate * 32.0, MaxCoordinate * 32.0);
        dy = std::clamp(dy, -MaxCoordinate * 32.0, MaxCoordinate * 32.0);
        r.left += dx; r.right += dx;
        r.top += dy; r.bottom += dy;
        Store(Constrain(r, viewport), base);
    }
};
}
