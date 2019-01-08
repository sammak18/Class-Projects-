/* Your code here! */
#include "dsets.h"

DisjointSets::DisjointSets(){

}


void DisjointSets::addelements	(	int 	num	){

    while(num>0){
      nodes.push_back(-1);
      num--;
    }
}

int DisjointSets::find	(	int 	elem	){

  int parent= _findhelper(elem);

  if (nodes[elem] >= 0){
    nodes[elem]=parent;
  }
  return parent;
}

int DisjointSets::_findhelper(	int 	elem	){

  if ( nodes[elem] < 0 ) {
    return elem; }
  else {
    return find( nodes[elem] ); }

}


void DisjointSets::setunion	(	int 	a, int 	b ){

  int parent_a = find(a);
  int parent_b = find(b);

  if(parent_b==parent_a){
    return;
  }

  if(nodes[parent_a]<=nodes[parent_b]){
    nodes[parent_a]=nodes[parent_a]+nodes[parent_b];
    nodes[parent_b]=parent_a;

  }
  else{
    nodes[parent_b]=nodes[parent_b]+nodes[parent_a];
    nodes[parent_a]=parent_b;
  }

}

int DisjointSets::size	(	int 	elem	){

  return nodes.size();

}
