#include <algorithm>
#include <vector>
#include <queue>
#include <stack>
#include <cstdlib>
#include <sys/time.h>
#include <time.h>
#include "maze.h"
#include "dsets.h"
#include <stdlib.h>
#include <iostream>
#include <map>

using namespace cs225;

//SquareMaze constructor. Creates Empty maze.
SquareMaze::SquareMaze(){
  maze_=DisjointSets();
}

//Makes a new SquareMaze of the given height and width.
//If this object already represents a maze it will clear all the existing data before doing so.
void 	SquareMaze::makeMaze (int width, int height){

  struct timeval time;
  gettimeofday(&time, NULL);
  srand((time.tv_sec * 1000) + (time.tv_usec / 1000));


  height_=height;
  width_=width;
  if (height < 2 || width < 2 ) {
        return;
    }


  for(int i=0;i<height*width;i++){
    rightwallexists.push_back(true);
    downwallexists.push_back(true);
  }

  maze_.nodes.clear();
  maze_.addelements(height*width);

  //srand(time(NULL));

  int exit =0;
  int n =height*width;

  while(n>1){
    if(exit==height*width-1){
      break;
    }

    int rand1= rand()%(height*width);
    int rand2= (rand()%2)+1;

    if (rand2==1){
      if((rand1+1)%width!=0 && maze_.find	(	rand1	)!=maze_.find	(	rand1	+1) ){
        exit++;
        rightwallexists[rand1]=false;
        maze_.setunion	(	rand1, rand1+1 );
        n=n-1;
      }

    }
    else{

      if(rand1+width<width*height  && maze_.find(rand1)!=maze_.find	(rand1+width)){
        exit++;
        downwallexists[rand1]=false;
        maze_.setunion	(	rand1, rand1+width );
        n=n-1;

      }
    }



  }

}


//This uses your representation of the maze to determine whether it is possible to travel
//in the given direction from the square at coordinates (x,y).
bool 	SquareMaze::canTravel (int x, int y, int dir) const{
    if(x==width_ || y==height_){
      return false;
    }

    if(dir==0){
      return (!rightwallexists[getCoord(x,y)]);
    }
    if(dir==1){
      return (!downwallexists[getCoord(x,y)]);
    }
    if(dir==2){
      if(x-1<0){
        return false;
      }
      else{
        return (!rightwallexists[getCoord(x-1,y)]);
      }
    }
    if(dir==3){
      if(y-1<0){
        return false;
      }
      else{
        return (!downwallexists[getCoord(x,y-1)]);
      }
    }
    return false;
  }

//Helper function to convert coordinates into 1D index.
int SquareMaze::getCoord(int x,int y)const{
  return y*width_+x;;
}

//Sets whether or not the specified wall exists.
void 	SquareMaze::setWall (int x, int y, int dir, bool exists){
  if(x==width_ || y==height_){
    return;
  }
  if (dir==0)
    {rightwallexists[getCoord(x,y)]=exists;
      return;}
  if(dir==1){
    downwallexists[getCoord(x,y)]=exists;
      return;
  }

}




//For each square on the bottom row (maximum y coordinate), there is a distance from the origin
//(i.e. the top-left cell), which is defined as the length (measured as a number of steps) of the
//only path through the maze from the origin to that square.
vector< int > 	SquareMaze::solveMaze (){
  std::vector<int> path;
  std::vector<int> result;
  //int current_max=0;


  std::vector<bool > visited;
  for(int j=0;j<height_*width_;j++){
    visited.push_back(false);
  }

  queue <pair<int,int>> queuey;

  pair<int,int> p;
  pair<int,int> q;
  q.first=0;
  q.second=0;
  std::map<int, int> myparent;

  std::map<int, int> distance;
  distance[0]=0;
  int dist=0;

  queuey.push(q);
  visited[0]=true;

  while(!queuey.empty()){

    dist++;
    pair<int,int> temp= queuey.front();
    int x = temp.first;
    int y = temp.second;
    queuey.pop();



    if (canTravel (x, y, 0) && visited[getCoord(x+1,y)]==false){
      visited[getCoord(x+1,y)]=true;
      myparent[getCoord(x+1,y)]=getCoord(x,y);
      distance[getCoord(x+1,y)]=dist;
      p.first=x+1;
      p.second=y;
      queuey.push(p);
    }

    if(canTravel (x, y, 1) && visited[getCoord(x,y+1)]==false){
      visited[getCoord(x,y+1)]=true;
      myparent[getCoord(x,y+1)]=getCoord(x,y);
      distance[getCoord(x,y+1)]=dist;
      p.first=x;
      p.second=y+1;
      queuey.push(p);
      }

    if(canTravel (x, y, 2) && visited[getCoord(x-1,y)]==false){
      visited[getCoord(x-1,y)]=true;
      myparent[getCoord(x-1,y)]=getCoord(x,y);
      distance[getCoord(x-1,y)]=dist;
      p.first=x-1;
      p.second=y;
      queuey.push(p);
      }

    if(canTravel (x, y, 3) && visited[getCoord(x,y-1)]==false){
      visited[getCoord(x,y-1)]=true;
      myparent[getCoord(x,y-1)]=getCoord(x,y);
      distance[getCoord(x,y-1)]=dist;
      p.first=x;
      p.second=y-1;
      queuey.push(p);
      }






  }

      int currentmax=0;
      int maximumXvalue=0;
      for(int xx=0;xx<width_;xx++){
       int current=getCoord(xx,height_-1);
         if (distance[current]==currentmax){
           if(xx<maximumXvalue){
             maximumXvalue=xx;//
           }
         }
          if (distance[current]>currentmax){//if distnace[current]==currentmax
            currentmax=distance[current];
            maximumXvalue=xx;//
          }
      }

    int ending=getCoord(maximumXvalue,height_-1);
    destinationcoords.first =maximumXvalue;
    destinationcoords.second = height_-1;


    while(ending!=0){


        if(ending-myparent[ending]==width_){
          result.push_back(1);
        }
        if(ending+width_==myparent[ending]){

          result.push_back(3);
        }
        if(ending+1==myparent[ending]){

          result.push_back(2);
        }
        if(ending-1==myparent[ending]){

          result.push_back(0);
        }
        ending=myparent[ending];
      }


  if (result.size()>0)
    {std::reverse(result.begin(),result.end());}


  return result;
}
//Draws the maze without the solution.
PNG * 	SquareMaze::drawMaze () const{

  PNG* pic=new PNG(width_*10+1,height_*10+1);
  for(int x=0;x<int(pic->width());x++){
    if(x<10){
      continue;
    }
    HSLAPixel &p=pic->getPixel(x,0);
    p.l=0;
  }
  for(int y=0;y<int(pic->height());y++){
    HSLAPixel &p=pic->getPixel(0,y);
    p.l=0;
  }
  for(int i=0;i<height_;i++){
    for(int j=0;j<width_;j++){
      if (rightwallexists[getCoord(i,j)]==true){
        for(int k=0;k<=10;k++){
          HSLAPixel &p=pic->getPixel((i+1)*10,j*10+k);
          p.l=0;
        }
      }
      if (downwallexists[getCoord(i,j)]==true){
        for(int k=0;k<=10;k++){
          HSLAPixel &p=pic->getPixel(i*10+k,(j+1)*10);
          p.l=0;
        }
      }
    }
  }
  return pic;
}



//Draws the maze with solution.
PNG * 	SquareMaze::drawMazeWithSolution (){

  PNG* drawn_maze = drawMaze();
  vector< int > path= solveMaze();

  int dir;

  int x=5;
  int y=5;
  HSLAPixel &p=drawn_maze->getPixel(x,y);
  p.h =0;
  p.s =1;
  p.l =0.5;
  p.a = 1;
  for(int i=0;i<int(path.size());i++){
    dir=path[i];
    //right
    for(int j=0;j<10;j++){
      if(dir==0){
        x=x+1;
        HSLAPixel &p=drawn_maze->getPixel(x,y);
        p.h =0;
        p.s =1;
        p.l =0.5;
        p.a = 1;
      }
      //down
      if(dir==1){
        y=y+1;
        HSLAPixel &p=drawn_maze->getPixel(x,y);
        p.h =0;
        p.s =1;
        p.l =0.5;
        p.a = 1;
      }
      //left
      if(dir==2){
        x=x-1;
        HSLAPixel &p=drawn_maze->getPixel(x,y);
        p.h =0;
        p.s =1;
        p.l =0.5;
        p.a = 1;
      }
      //up
      if(dir==3){
        y=y-1;
        HSLAPixel &p=drawn_maze->getPixel(x,y);
        p.h =0;
        p.s =1;
        p.l =0.5;
        p.a = 1;
    }
    }

  }

  //making the exit
  for(int k=1;k<10;k++){
    HSLAPixel &p=drawn_maze->getPixel(destinationcoords.first*10+k, (destinationcoords.second+1)*10);
    p.l =1;
  }


  return drawn_maze;
}

//Drawing creative maze function
PNG * SquareMaze::drawCreativeMaze(){

    for(int j=(height_/3);j<int((2*height_)/3);j++){
      for(int i=0;i<int(width_/3);i++){
        rightwallexists[getCoord(i,j)]=true;
        downwallexists[getCoord(i,j)]=true;
      }
    }

    for(int j=(height_/3);j<int((2*height_)/3);j++){
      for(int i=int((2*width_)/3);i<width_;i++){
        rightwallexists[getCoord(i,j)]=true;
        downwallexists[getCoord(i,j)]=true;
      }
    }
    PNG* drawn_maze = drawMaze();
    vector< int > path= solveMaze();

    int dir;

    int x=5;
    int y=5;
    //HSLAPixel &p=drawn_maze->getPixel(x,y);
    HSLAPixel &p=drawn_maze->getPixel(x,y);
    p.h =0;
    p.s =1;
    p.l =0.5;
    p.a = 1;
   //red 0,1,0.5,1
    for(int i=0;i<int(path.size());i++){
      dir=path[i];
      //right
      for(int j=0;j<10;j++){
        if(dir==0){
          x=x+1;
          HSLAPixel &p=drawn_maze->getPixel(x,y);
          p.h =0;
          p.s =1;
          p.l =0.5;
          p.a = 1;
        }
        //down
        if(dir==1){
          y=y+1;
          HSLAPixel &p=drawn_maze->getPixel(x,y);
          p.h =70;
          p.s =1;
          p.l =0.5;
          p.a = 1;
        }
        //left
        if(dir==2){
          x=x-1;
          HSLAPixel &p=drawn_maze->getPixel(x,y);
          p.h =140;
          p.s =1;
          p.l =0.5;
          p.a = 1;
        }
        //up
        if(dir==3){
          y=y-1;
          HSLAPixel &p=drawn_maze->getPixel(x,y);
          p.h =210;
          p.s =1;
          p.l =0.5;
          p.a = 1;
      }
      }

    }

    //making the exit
    for(int k=1;k<10;k++){
      HSLAPixel &p=drawn_maze->getPixel(destinationcoords.first*10+k, (destinationcoords.second+1)*10);
      p.l =1;
    }
    return drawn_maze;

}

//Make creative maze function called in main.cpp
void 	SquareMaze::makeCreativeMaze (int width, int height){

  height_=height;
  width_=width;
  if (height < 2 || width < 2 ) {
        return;
    }


  for(int i=0;i<height*width;i++){
    rightwallexists.push_back(true);
    downwallexists.push_back(true);
  }

  maze_.nodes.clear();
  maze_.addelements(height*width);

  //srand(time(NULL));

  int exit =0;
  int n =height*width;

  while(n>1){
    if(exit==height*width-1){
      break;
    }

    int rand1= rand()%(height*width);
    int rand2= (rand()%2)+1;

    if (rand2==1){
      if((rand1+1)%width!=0 && maze_.find	(	rand1	)!=maze_.find	(	rand1	+1) ){
        exit++;
        rightwallexists[rand1]=false;
        maze_.setunion	(	rand1, rand1+1 );
        n=n-1;
      }

    }
    else{

      if(rand1+width<width*height  && maze_.find(rand1)!=maze_.find	(rand1+width)){
        exit++;
        downwallexists[rand1]=false;
        maze_.setunion	(	rand1, rand1+width );
        n=n-1;

      }
    }



  }

}
