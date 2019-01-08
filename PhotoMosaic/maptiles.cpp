/**
 * @file maptiles.cpp
 * Code for the maptiles function.
 */

#include <iostream>
#include <map>
#include "maptiles.h"

using namespace std;

Point<3> convertToLAB(HSLAPixel pixel) {
    Point<3> result(pixel.h/360, pixel.s, pixel.l);
    return result;
}

/**
* This function takes a SourceImage and a vector of TileImages
* and returns a MosaicCanvas pointer. It maps the rectangular regions of the
* SourceImage to TileImages.
**/

MosaicCanvas* mapTiles(SourceImage const& theSource,
                       vector<TileImage>& theTiles)
{
    /**
     * @todo Implement this function!
     */
     int rows=theSource.getRows();
     int cols=theSource.getColumns();
     std::cout << "row: " << rows<< '\n';
     std::cout << "column: " << cols<< '\n';

     MosaicCanvas* canvas=new MosaicCanvas(rows,cols);

     vector<Point<3>> v1;
     map<Point<3>,TileImage*> m1;
     //std::cout << "line30" << '\n';
     for (int n=0; n<int(theTiles.size());n++){
       HSLAPixel pixel=theTiles[n].getAverageColor();
       Point<3> color;
       color[0]=pixel.h;
       color[1]=pixel.s;
       color[2]=pixel.l;
       v1.push_back(color);
       m1[color]=&theTiles[n];
     }
     //std::cout << "line40" << '\n';

     KDTree<3> tree(v1);
     //std::cout << "line43" << '\n';
     TileImage vela;
     for(int i=0;i<cols;i++){
       for (int j=0;j<rows;j++){
         HSLAPixel p=theSource.getRegionColor(j,i);
         Point<3> color;
         color[0]=p.h;
         color[1]=p.s;
         color[2]=p.l;

         //TileImage * temp=&vela;

         canvas->setTile(j,i,m1[tree.findNearestNeighbor(color)]);
       }
     }

     //std::cout << "line56" << '\n';


    return canvas;
}

TileImage* get_match_at_idx(const KDTree<3>& tree,
                                  map<Point<3>, int> tile_avg_map,
                                  vector<TileImage>& theTiles,
                                  const SourceImage& theSource, int row,
                                  int col)
{
    // Create a tile which accurately represents the source region we'll be
    // using
    HSLAPixel avg = theSource.getRegionColor(row, col);
    Point<3> avgPoint = convertToLAB(avg);
    Point<3> nearestPoint = tree.findNearestNeighbor(avgPoint);

    // Check to ensure the point exists in the map
    map< Point<3>, int >::iterator it = tile_avg_map.find(nearestPoint);
    if (it == tile_avg_map.end())
        cerr << "Didn't find " << avgPoint << " / " << nearestPoint << endl;

    // Find the index
    int index = tile_avg_map[nearestPoint];
    return &theTiles[index];

}
