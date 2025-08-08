# No Tracking Markers

---

This application automatically removes those pesky tracking markers from URLs in your clipboard.

Windows only for now. Maybe I'll make a linux version later.

### Building from source
Requires CMake & Boost. Use vcpkg if you have it. I built this for MSVC so YMMV with other toolchains.

**I recommend just using your IDE's built in build system.**<br>
Building with the command line follows the usual steps:
1. `mkdir build; cd build`
2. `cmake .. -G Ninja`
3. `ninja -j4`

---
<sub>Licensed Under GNU GPL-v3.0</sub>