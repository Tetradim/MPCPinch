# MPCPinch

An unofficial MPC-HC build with pointer-centered video zoom and drag-to-pan,
for examining details in playing or paused videos.

Based on [MPC-HC 2.8.3](https://github.com/clsid2/mpc-hc/releases/tag/2.8.3),
upstream commit `7bcb746166de30297222a3f5f6d49d747dd7dd51`.
Original documentation: [README.upstream.md](README.upstream.md).

## Download

Get the Windows x64 portable ZIP from [Releases](https://github.com/Tetradim/MPCPinch/releases).
Extract the entire ZIP to a writable folder, then run `mpc-hc64.exe`.
Keep the included DLLs and `mpc-hc64.ini` together. The INI keeps this copy's
settings separate from an installed MPC-HC. The bundled MPC Video Renderer is
selected by default.

To update an existing MPCPinch 2.8.3 portable copy while keeping your settings,
close it and replace its `mpc-hc64.exe` with the executable from revision 2.
Launch that copy explicitly if other installed players have your file associations.

## Controls

- Point at a detail and pinch outward/inward to zoom from 1x to 8x.
- Ctrl+mouse-wheel also zooms around the pointer.
- Ctrl+= / Ctrl+- are keyboard zoom alternatives.
- While zoomed, hold the left touchpad button and drag to pan. Dragging keeps a
  paused video paused. Tap-and-drag works when enabled in Windows.
- Ctrl+0 resets inspection zoom. Pan&Scan Reset also resets it.
- Opening another video resets inspection zoom.

Pinch depends on the touchpad driver sending Windows Ctrl+wheel events. Physical
pinch compatibility has not been verified on every device. System EVR, DVD menus
and interactive video are excluded from these inspection controls.

Revision 2 includes `.m3u8` HLS manifests in Next/Previous folder navigation.
Their MPEG-TS segments play as one continuous video; Next/Previous moves to the
neighboring file in the folder.

## Build and tests

```console
git clone --recurse-submodules https://github.com/Tetradim/MPCPinch.git
cd MPCPinch
```

Install Visual Studio 2022 C++ build tools with ATL/MFC, Windows SDK 10.0.26100.0,
and NASM 2.16.03 on PATH. In an x64 Native Tools Command Prompt:

```console
msbuild mpc-hc.sln /t:Apps\mpc-hc /p:Configuration=Release /p:Platform=x64 /p:MPCHC_WINSDK_VER=10.0.26100.0 /m:4
cl /nologo /EHsc /std:c++17 /W4 tests/video_inspection_test.cpp /Fe:video_inspection_test.exe
video_inspection_test.exe
```

The executable is `bin/mpc-hc_x64/mpc-hc64.exe`. Use it with the supporting DLLs
from the upstream MPC-HC 2.8.3 x64 portable distribution.
GitHub's automatic source ZIPs omit submodule contents; use the recursive clone
or the complete source archive attached to the release.

See [the user guide](docs/TouchpadInspection.md),
[verification results and limitations](VERIFICATION.txt), and
[HLS navigation regression procedure](tests/folder_navigation.md).

## License and attribution

GPL v3 or later; see [COPYING.txt](COPYING.txt), [Authors.txt](docs/Authors.txt), and
the bundled third-party license notices. This repository preserves the upstream credits and records the exact upstream
version and commit used for this source snapshot. MPCPinch is an unofficial modification, not an official
MPC-HC release. Its executable is unsigned.
