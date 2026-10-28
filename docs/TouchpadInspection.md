# MPC-HC 2.8.3 — Touchpad Inspection Edition

This is an unofficial custom build of MPC-HC 2.8.3, based on upstream commit
7bcb746166de30297222a3f5f6d49d747dd7dd51. It adds video inspection controls.

## Controls

- Point at the detail you want to inspect and pinch outward/inward on the touchpad.
- Ctrl+mouse-wheel also zooms. Fine wheel deltas are supported; range is 1x to 8x.
- Ctrl+= / Ctrl+- zoom in/out around the pointer as keyboard alternatives.
- While zoomed, hold the left touchpad button and move your finger to pan.
  A stationary click keeps its usual playback action. Dragging does not resume
  a paused video. Touchpad tap-and-drag works if enabled in your Windows settings.
- Ctrl+0 resets inspection zoom. View > Pan&Scan > Reset (Numpad 5) also resets it.
- Opening another video resets inspection zoom.

Ctrl+wheel over the video now zooms instead of seeking. Ordinary wheel and
seekbar/toolbar controls retain their normal actions. Image edges constrain panning;
an axis that still fits inside the player remains centered.

## Folder navigation (revision 2)

Next and Previous now include local .m3u8 HLS manifests when browsing a folder.
Each manifest opens as one continuous video, with its MPEG-TS segments handled
by the existing HLS decoder. Another Next/Previous press moves to the adjacent
file in the folder. General wildcard playlist expansion retains its original
playlist exclusions.

## Run separately

Extract the entire portable ZIP to a writable folder, then run mpc-hc64.exe.
Keep the supplied mpc-hc64.ini next to it: that keeps this copy's settings separate
from the registry settings of an installed MPC-HC. The package selects the bundled
MPC Video Renderer. EVR Custom Presenter also uses the supported rendering path.
System EVR inspection is disabled because it does not use the same destination
rectangle. DVD menus and interactive video are excluded from inspection gestures.

This build is unsigned and unofficial. Official MPC-HC updates do not include these
changes. Keep this folder separate when installing official updates.

Touchpad pinch relies on Windows delivering Ctrl+wheel messages. A driver that does
not send those messages requires a different input adapter; Ctrl+wheel and the
keyboard alternatives provide the same video inspection behavior.

## Source and rebuilding

The source patch and source archive accompany this player. License: GPL v3 or later;
see COPYING.txt and upstream license notices for bundled components.

Upstream: https://github.com/clsid2/mpc-hc
Release assets: https://github.com/clsid2/mpc-hc/releases/tag/2.8.3

Install Visual Studio 2022 C++ x64/x86 build tools, ATL/MFC and Windows SDK 10.0.26100.0.
Put NASM 2.16.03 on PATH. From an x64 Native Tools Command Prompt in the source root:

    msbuild mpc-hc.sln /t:Apps\mpc-hc /p:Configuration=Release /p:Platform=x64 /p:MPCHC_WINSDK_VER=10.0.26100.0 /m:4

Use the resulting bin/mpc-hc_x64/mpc-hc64.exe with the unmodified supporting DLLs
from the upstream 2.8.3 x64 portable ZIP. This builds the full player, with internal
codec support; the codec DLLs themselves are the matching upstream release builds.

Geometry test (from source root, in the same developer prompt):

    cl /nologo /EHsc /std:c++17 /W4 tests/video_inspection_test.cpp /Fe:video_inspection_test.exe
    video_inspection_test.exe

See VERIFICATION.txt for the checks actually performed and remaining limitations.
