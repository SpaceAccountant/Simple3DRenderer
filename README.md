
Simple 3D
====

OVERVIEW
----
There are a two things to note if you plan on building and/or running this project:
- The "res" folder must be in your project directory or in the executable output directory
- This hasn't been build nor tested on macOS (OS X)
- If the characters in your build don't look correct then they probably aren't, but they can be changed in the CursesDevice.c file
- The non-Windows version has a more limited palette and so gradients won't look as good as they do on windows.

Something else that should be mentioned is despite the fact the game can only support one level, you can use a custom level.
Try running the game with the argument "res/maps/level2.map" (ie., run the command "C3D.exe res/maps/level2.map" or "./C3D.out res/maps/level2.map").
You can write your own custom map if you want, just see MAPFORMAT.txt for more information.

If you want more info or have questions, please contact me at claire.clark.muth@gmail.com

BUILDING
----
If you're only interested in getting a working binary of this project, please view the releases.

This project uses CMake as its build system.
Build presets follow the naming convention of **`<build type>-<generator>`**.
There are currently two predefined presets:
  - debug-make
  - release-make

To simply build the project:
  1. make sure you are in the project root directory
  2. run **`cmake --preset=<preset name> .`**, where **`<preset name>`** is one of the presets listed above
  3. run **`cmake --build build/<build type>`**, where **`<build type>`** is the build type of the preset used in the previous step

For example, a release build using UNIX Makefiles is built by running
```
cmake --preset=release-make .
cmake --build build/release
```

For more advanced build options, the following variables are available for further configuration:
  - `CMAKE_SYSTEM_PROCESSOR` defines the processor architecture of the build, determining where the build target artifacts get stored. This should be defined when cross compiling, as per the [CMake documentation](https://cmake.org/cmake/help/v3.21/variable/CMAKE_SYSTEM_PROCESSOR.html).
  - `PROJECT_RENDER_BACK_ENDS` defines the list of back-end rendering libraries to build into the application. The ones that ship with the code are:
    - `curses`: A rendering back-end for terminals built with the curses library. The build system automatically attempts to find the curses library, but it can explicitly be specified with the `PROJECT_CURSES_LIBRARY` variable.
