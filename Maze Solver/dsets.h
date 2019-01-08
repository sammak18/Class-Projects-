/* Your code here! */
#ifndef DSETS_H
#define DSETS_H

#include <random>
#include <stdio.h>
#include <vector>
#include <cstdio>
#include <climits>
#include <algorithm>
#include <string>
#include <iostream>

/*
Each DisjointSets object represents a family of disjoint sets, where each element has an integer index.

It is implemented with the optimizations discussed in class, as up-trees stored in a single vector of ints.
Specifically, path compression and union-by-size are used. Each place in the vector represents a node.
(Note that this means that the elements in our universe are indexed starting at 0.) A nonnegative number is
the index of the parent of the current node; a negative number in a root node is the negative of the set size.

Note that the default compiler-supplied Big Three will work flawlessly because the only member data is a vector
of integers and this vector should initially be empty.
*/

class DisjointSets{

  public:
    DisjointSets();

    std::vector<int> nodes;

    void addelements	(int num);

    int find	(	int elem);

    void setunion	(	int a, int b );

    int size	(	int elem );

    /*class Node{

      public:
        Node();

        int data;

        int rank;

        Node * parent;
    };*/

  private:
    int _findhelper( int elem);

};



#endif
