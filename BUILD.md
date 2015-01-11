# How to Build

## Dependencies
Almost all dependencies are included in the repository, though there are a couple of external things needed.

All platforms need a build system (XCode, Visual Studio, Clang, GCC, etc.) and CMake. In addition, in order to build on Linux, you will need the OpenGL development libraries (available in libgl1-mesa-dev) and the GTK+ 3 development libraries (available in libgtk-3-dev).

## Building
On your platform of choice, execute CMake to generate the content needed by your build system, then build the program.

## Requirements
Your build system must support most of the C++11 features. Unfortunately, that means that no Visual Studio version earlier than 2015 will work.
