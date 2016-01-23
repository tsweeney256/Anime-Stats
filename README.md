# Anime Stats

A tool to help keep track of all the shows you've watched and various individual and aggregate information about them.

Compile using CMake. In addition to the standard "Release" and "Debug" build modes, the CMake script also supports "Static_Release" and "Static_Debug" build modes that compile the project statically. You also must use -DWIN=true as an argument to compile the resources for Windows builds if you are cross compiling from a different operating system. The resources will be compiled automatically if building on Windows itself. The only supported compilers are c++14 compatible versions of GCC and w64-mingw32. Other compilers may work as well, but they have not been tested.
