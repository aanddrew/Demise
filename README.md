---Demise---

This is a 3d first-person maze crawling experience.

It is a 3d game rendered in the style of wolfenstein/doom 
using a binary space partition (bsp).

The game uses SFML to draw the 2d graphics, but 3d projection and perspective is done internally.

[Here is a short video explaining the project.](https://www.youtube.com/watch?v=i4UJuVc9Mvo&feature=youtu.be)

In order to build and run this project yourself, you will need to install [SFML.](https://www.sfml-dev.org/)
If you are on ubuntu, after installing SFML you can go directly into the "showcase" folder and do ./build3d.sh && ./a.out
This will build and execute the project.

If you are on windows or mac I don't really know how to help you but the main function is inside the "test3d.cpp" file in the showcase folder. The files needed to compile are all outlined in the build3d.sh script in that folder as well.

Game engine and graphics by Andrew Weller,
Level Design by Trey Nevitt.
