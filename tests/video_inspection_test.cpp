#include "../src/mpc-hc/VideoInspection.h"
#include <cmath>
#include <iostream>
#include <limits>

using VideoInspection::Rect;
using VideoInspection::Transform;
static int failures = 0;
#define CHECK(expr) do { if (!(expr)) { std::cerr << __LINE__ << ": " #expr "\n"; ++failures; } } while (false)
static bool Near(double a, double b) { return std::abs(a - b) < 1e-8; }
static bool Same(Rect a, Rect b) {
    return Near(a.left,b.left) && Near(a.top,b.top) && Near(a.right,b.right) && Near(a.bottom,b.bottom);
}

int main() {
    const Rect view{0,0,1000,800};
    Transform t;
    CHECK(!t.Active());
    CHECK(Same(t.Apply(view,view), view));
    CHECK(t.ZoomAt(120,300,200,view,view));
    CHECK(t.Active());
    CHECK(Near(t.Zoom(),1.1));
    Rect r = t.Apply(view,view);
    CHECK(Near(r.left,-30) && Near(r.top,-20));
    CHECK(Near((300-r.left)/r.width(),0.3));
    CHECK(Near((200-r.top)/r.height(),0.25));
    CHECK(t.ZoomAt(240,300,200,view,view));
    CHECK(t.ZoomAt(-240,300,200,view,view));
    CHECK(Same(t.Apply(view,view),r));
    CHECK(t.ZoomAt(-120,300,200,view,view));
    CHECK(!t.Active() && t.Zoom() == 1.0);
    CHECK(Same(t.Apply(view,view),view));
    CHECK(!t.ZoomAt(-120,300,200,view,view));

    CHECK(t.ZoomAt(1,500,400,view,view));
    CHECK(t.Zoom() > 1 && t.Zoom() < 1.1);
    t.Reset();
    CHECK(!t.Active() && Same(t.Apply(view,view),view));
    CHECK(t.ZoomAt(1e300,500,400,view,view));
    CHECK(t.Zoom() == 8);
    CHECK(!t.ZoomAt(120,500,400,view,view));
    CHECK(t.ZoomAt(-1e300,500,400,view,view));
    CHECK(t.Zoom() == 1);

    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double inf = std::numeric_limits<double>::infinity();
    CHECK(!t.ZoomAt(nan,500,400,view,view));
    CHECK(!t.ZoomAt(inf,500,400,view,view));
    CHECK(!t.ZoomAt(120,nan,400,view,view));
    CHECK(!t.ZoomAt(120,500,400,Rect{0,0,0,800},view));
    CHECK(!t.ZoomAt(120,500,400,view,Rect{0,0,inf,800}));
    CHECK(!t.Active());
    CHECK(!t.ZoomAt(120,500,400,Rect{0,0,1e308,800},view));
    CHECK(!t.Active());

    // Preserve the existing destination exactly at 1x, including Pan & Scan.
    const Rect original{-80,100,1120,700};
    CHECK(Same(t.Apply(original,view),original));
    t.Pan(100,100,original,view);
    CHECK(Same(t.Apply(original,view),original));
    CHECK(t.ZoomAt(120,300,400,original,view));
    r = t.Apply(original,view);
    CHECK(Near(r.top,70) && Near(r.bottom,730));
    t.Pan(0,1000,original,view);
    CHECK(Same(t.Apply(original,view),r));
    t.Reset();

    const Rect portrait{300,0,700,800};
    CHECK(t.ZoomAt(120,310,200,portrait,view));
    r = t.Apply(portrait,view);
    CHECK(Near(r.left,280) && Near(r.right,720));
    CHECK(Near(r.top,-20));
    t.Pan(1000,0,portrait,view);
    CHECK(Same(t.Apply(portrait,view),r));
    t.Reset();

    CHECK(t.ZoomAt(120,0,0,view,view));
    r = t.Apply(view,view);
    CHECK(Near(r.left,0) && Near(r.top,0));
    t.Reset();

    CHECK(t.ZoomAt(120,300,200,view,view));
    t.Pan(1e300,1e300,view,view);
    r = t.Apply(view,view);
    CHECK(Near(r.left,0) && Near(r.top,0));
    t.Pan(-1e300,-1e300,view,view);
    r = t.Apply(view,view);
    CHECK(Near(r.right,1000) && Near(r.bottom,800));
    t.Pan(nan,0,view,view);
    CHECK(Same(t.Apply(view,view),r));
    t.Pan(0,inf,view,view);
    CHECK(Same(t.Apply(view,view),r));
    t.Reset();

    CHECK(t.ZoomAt(120,300,200,view,view));
    t.Pan(-10,-15,view,view);
    r = t.Apply(view,view);
    // Rejected events must leave an already active inspection unchanged.
    const double savedZoom = t.Zoom();
    CHECK(!t.ZoomAt(0,300,200,view,view));
    CHECK(!t.ZoomAt(120,300,200,view,Rect{0,800,1000,0}));
    CHECK(!t.ZoomAt(120,300,200,Rect{-1e308,0,1e308,800},view));
    CHECK(!t.ZoomAt(-120,inf,200,view,view));
    t.Pan(20,20,view,Rect{0,0,0,800});
    t.Pan(20,20,Rect{0,0,nan,800},view);
    CHECK(t.Zoom() == savedZoom);
    CHECK(Same(t.Apply(view,view),r));
    Rect resized = t.Apply(Rect{100,50,2100,1650},Rect{100,50,2100,1650});
    CHECK(Near(resized.left,100 + 2*r.left));
    CHECK(Near(resized.top,50 + 2*r.top));
    CHECK(Near(resized.width(),2*r.width()));
    CHECK(t.ZoomAt(-1e300,300,200,view,view));
    CHECK(Same(t.Apply(view,view),view));
    std::cout << (failures ? "FAIL: " : "PASS: ") << failures << " failures\n";
    return failures ? 1 : 0;
}
