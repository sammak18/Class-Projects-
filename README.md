# Class-Projects-
This repository contains some of the projects I developed for my classes at the University of Illinois at Urbana-Champaign <br />

1. Photo Mosaic (CS 225): Generates a photo-mosaic which consists of an image being made from several smaller images. The algorithm 
averages the RGB color values for all the pixels in a given sized-block and then uses k-d trees constructed of all the images
in the database to find the image which resembles the pixel's colour most closely. A final mosaic generated code is attached 
labelled as "mymosaic.png".<br />

Programming language involved: C++.<br />
Files modified: kdtree.cpp, kdtree.h

2. Maze generator and Solver (CS 225): Generates a random maze of a given size using disjoint sets and solves it by performing a 
Breath-First Search and storing a particular block's 'parent block'. Once the end-point is reached, the path can be 
traced back to the start point and drawn on the png using the 'parent blocks' being stored. A few mazes generated and 
solved from the algorithm can be found as 'testDrawSolutionMed.png' and 'testDrawSolutionLarge.png'.

Programming language involved: C++. <br />
Files modified: dsets.cpp, dsets.h, maze.cpp, maze.h


3. Missile Command (ECE 391): Recreated Atari's Missile Command (1980) in x86 assembly language. The game involves protecting your base
from enemy missiles by shooting friendly missiles that cause the enemy missiles to explode. The friendly missiles are shot using a crosshair
and have a specified blast radius. The program draws the objects by putting the appropriate data in video memory, traverses a linked list of enemy missiles
and asynchronously handles the user input for the game using a jumptable. 

Programming language involved: x86 Assembly Language.<br />
Files modified: mp1.S<br />

4. Adventure (ECE 391): Added multiple functionalities to an adventure-quest interactive game:

    i.   Movement across the image: Enabled movement across the image using arrow keys by redrawing the parts of the image being traversed by the adventurer.<br />
    ii.  Tux Controller: Added functionality to use input from a 'Tux Controller' (Similar to a video game controller) which contained buttons and LED lights
         indicating the time game has running.<br />
    iii. Graphics: Improved graphics by using octrees to select the best 128 colours to represent from the 4096 colours generated for the image and transferring them in the colour          palette.<br />
    iv.  Keyboard input and location display : Added support for the current players's location and user input to be echoed on the screen in word-art font. <br />


Programming language involved: C, x86 assembly language.<br />
Files modified: photo.c, photo.h, modex.c, modex.h, adventure.c, input.c, module/mtcp.h, module/tuxctl-ioctl.c, module/tuxctl-ioctl.h, module/tuxctl-ld.c,
module/tuxctl-ld.h<br />


Acknowledgements:
    Yashovardhan Arora: For working as a partner in the Maze generator and Solver Project.