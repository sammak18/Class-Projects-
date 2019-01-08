/* Your code here! */
/* Your code here! */
#ifndef MAZE_H
#define MAZE_H

#include <random>
#include <stdio.h>
#include <cstdio>
#include <climits>
#include <algorithm>
#include <string>
#include <iostream>
#include <vector>
#include "cs225/PNG.h"
#include "cs225/HSLAPixel.h"
#include "dsets.h"


class SquareMaze{

  public:
    SquareMaze();
    void 	makeMaze (int width, int height);
    bool 	canTravel (int x, int y, int dir) const;
    void 	setWall (int x, int y, int dir, bool exists);
    vector< int > 	solveMaze ();
    cs225::PNG * 	drawMaze () const;
    cs225::PNG * 	drawMazeWithSolution ();
    int height_;
    int width_;
    int getCoord(int x,int y) const;
    cs225::PNG * 	drawCreativeMaze();
    void 	makeCreativeMaze (int width, int height);

  private:
    DisjointSets maze_;
    bool weredone();
    vector<bool> rightwallexists;
    vector<bool> downwallexists;
    pair<int,int> destinationcoords;


};





#endif
