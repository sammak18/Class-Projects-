#include <iostream>
#include "dsets.h"
#include "maze.h"
#include "cs225/PNG.h"

using namespace std;
/*
Using function to make a creative maze
*/
int main()
{

    SquareMaze creative_mazeDS=  SquareMaze();
    creative_mazeDS.makeCreativeMaze ( 50, 50);
    cs225::PNG* creative_maze = creative_mazeDS.drawCreativeMaze();
    creative_maze->writeToFile("creative.png");

    return 0;
}
