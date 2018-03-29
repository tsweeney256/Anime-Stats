# Anime Stats

A multi-platform application to help keep track of all the shows you've watched and various individual and aggregate statistics about them. Supports importing your data from MAL.

![](https://raw.githubusercontent.com/tsweeney256/Anime-Stats/images/1.png)

## Download and Install
### Windows:
[Download portable executable here](https://github.com/tsweeney256/Anime-Stats/releases)

### Linux, Mac, BSD:
#### Dependencies
- GCC, Clang, or MinGW-w64
- CMake 3.3 or higher
- wxWidgets 3.0
- SQLite (optional)
#### Download and Compile
```bash
git clone --recursive https://github.com/tsweeney256/Anime-Stats.git
cd Anime-Stats
mkdir build
cd build
cmake ..
make
sudo make install
```
## CMake Options
- Build Types: release (default), debug, static_release, static_debug
- USE_AMALGAMATION: Use sqlite amalgamation packaged in this repository. true (default on mac), false (default everywhere else)
- PROFILING: Use profiling friendly compile options. Does not set -pg. true, false (default)
