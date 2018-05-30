There are a two things to note if you plan on building and/or running this project:
- The "res" folder must be in your project directory or in the executable output directory
- This hasn't been build nor tested on macOS (OS X)
- If the characters in your build don't look correct then they probably aren't, but they can be changed in the CursesDevice.c file
- The non-Windows version has a more limited palette and so gradients won't look as good as they do on windows.

Something else that should be mentioned is despite the fact the game can only support one level, you can use a custom level.
Try running the game with the argument "res/maps/level2.map" (ie., run the command "C3D.exe res/maps/level2.map" or "./C3D.out res/maps/level2.map").
You can write your own custom map if you want, just see MAPFORMAT.txt for more information.

The license is in LICENSE.txt

If you want more info or have questions contact jaidenthedoctor@gmail.com
