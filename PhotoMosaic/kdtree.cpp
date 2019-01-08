/**
 * @file kdtree.cpp
 * Implementation of KDTree class.
 */

#include <utility>
#include <algorithm>
#include <queue>
#include <stack>
#include <climits>
using namespace std;

/**
* This function checks if the first point has a smaller
* value value than the second point in the dimensions specified.
**/

template <int Dim>
bool KDTree<Dim>::smallerDimVal(const Point<Dim>& first,
                                const Point<Dim>& second, int curDim) const
{
    if (first[curDim]< second[curDim]){
      return true;
    }
    else if(first[curDim]== second[curDim]){
        if (first<second){
          return true;
        }
        else{
          return false;
        }
    }
    else{
      return false;}
}
/**
* This function return strue if potential is closer (i.e., has a smaller distance)
* to target than currentBest (with a tie being broken by the operator< in the
* Point class).
**/
template <int Dim>
bool KDTree<Dim>::shouldReplace(const Point<Dim>& target,
                                const Point<Dim>& currentBest,
                                const Point<Dim>& potential) const
{

     double sum1=0;
     double sum2=0;
     for(int i=0; i<Dim; i++){
       sum1+=pow((target[i]-potential[i]),2);
       sum2+=pow((target[i]-currentBest[i]),2);
     }
     if(sum1<sum2)
      return true;
     else if(sum1>sum2)
      return false;
     else
      return (potential<currentBest);

}
/**
* This is a constructor that accepts a vector of points
* and constructs a new KDTree with it.
**/
template <int Dim>
KDTree<Dim>::KDTree(const vector<Point<Dim>>& newPoints)
{
    if(newPoints.size()!=0){
    vector<Point<Dim>> v_copy;


      for (int i=0;i<int(newPoints.size());i++){
        v_copy.push_back(newPoints[i]);
      }
      root=bob_builder(v_copy,0);
    }
    else{
      root=NULL;
    }


}
/**
* This is a helper function for the constructor that recursively builds a KDTree
* by accepting a a dimension value.
**/


template <int Dim>
typename KDTree<Dim>::KDTreeNode* KDTree<Dim>::bob_builder(vector<Point<Dim>> copyvector, int d){

  if(copyvector.size()==0){
    return NULL;
  }
  else if(copyvector.size()==1){
    KDTreeNode *newnode= new KDTreeNode(copyvector[0]);
    return newnode;
  }

  int median;
  if (copyvector.size()%2!=0)
    median = (copyvector.size()+1)/2;
  else
    median = copyvector.size()/2;

  int newdim=(d+1)%Dim;

  Point<Dim> median_point = quick_select(copyvector,0,copyvector.size()-1,median,d);

  vector<Point<Dim>> left_vector;
  for (int i=0;i<median-1;i++){
    left_vector.push_back(copyvector[i]);
  }


  vector<Point<Dim>> right_vector;
  for (int i=median;i<int(copyvector.size());i++){
    right_vector.push_back(copyvector[i]);
  }


  KDTree<Dim>::KDTreeNode* newnode= new KDTreeNode(copyvector[median-1]);

  newnode->left=bob_builder(left_vector,newdim);
  newnode->right=bob_builder(right_vector,newdim);


  return newnode;


}

/**
* It selects  the medianth smallest no from the given vector between the low and height
* indices which is essentially the median;
**/

template <int Dim>
Point<Dim> KDTree<Dim>::quick_select(vector<Point<Dim>>& Points, int low, int high, int median, int d){

  if (low==high){


    return Points[low];
  }

  int pivotindex = partition(Points, low, high,d);


  int length = pivotindex-low+1;
  if (length==median){

    return Points[pivotindex];}
  else if(median< length){

    return quick_select(Points, low, pivotindex - 1,median ,d);}
  else{

    return quick_select(Points, pivotindex + 1, high, median - length,d);
  }




}

/**
* This function sorts the vector in such a way that all the elements
* to the right of the median are larger than it and all the elements
* to the left of the median are smealler than it.
**/

template <int Dim>
int KDTree<Dim>::partition (vector<Point<Dim>>& Points, int low, int high,int d)
{

    double pivot = Points[high][d];



    while(low<high)
    {

        while(Points[low][d] < pivot)
          low++;

        while ( Points[high][d] > pivot )
          high--;

        if ( Points[low][d] == Points[high][d] )
            low++;
        else if (low<high)
        {

            Point<Dim> temp = Points[low];
            Points[low] = Points[high];
            Points[high]= temp;

        }
    }


    return high ;
}
/**
* The copy constructor for KDTree class
**/
template <int Dim>
KDTree<Dim>::KDTree(const KDTree& other) {
  /**
   * @todo Implement this function!
   */
   root=_copy(other.root);
}
/**
* The assignment operator for KDTree class
**/
template <int Dim>
const KDTree<Dim>& KDTree<Dim>::operator=(const KDTree& rhs) {
  /**
   * @todo Implement this function!
   */
   if(this!=&rhs){
     _destroy(root);
     root=_copy(rhs.root);
   }

  return *this;
}

/**
* The destructor for KDTree class
**/
template <int Dim>
KDTree<Dim>::~KDTree() {
  /**
   * @todo Implement this function!
   */
   _destroy(this->root);
}

template <int Dim>
void KDTree<Dim>:: _destroy(KDTreeNode * subroot){
  if(subroot==NULL){
    return;
  }

  _destroy(subroot->left);

  _destroy(subroot->right);

  delete subroot;
}
/**
* Recursive helper function to copy a tree given it's subroot.
**/
template <int Dim>
typename KDTree<Dim>::KDTreeNode* KDTree<Dim>::_copy(const KDTreeNode* subroot){
  if (subroot==NULL)
    return NULL;
  KDTreeNode* newnode=new KDTreeNode(subroot->point);
  newnode->left=_copy(subroot->left);
  newnode->right=_copy(subroot->right);
  return newnode;
}

/**
* This function accepts a query point and finds a point closes to it in the KDTree.
**/

template <int Dim>
Point<Dim> KDTree<Dim>::findNearestNeighbor(const Point<Dim>& query) const
{
    int dimension =0;

    int max = INT_MAX;
    int &best_distance_sqrd = max;
    KDTreeNode* copy;// = this->root;
    KDTreeNode *& best_node = copy;

    best_node = walker( this->root, query, dimension, best_distance_sqrd ,best_node);

    Point<Dim> ret_val = best_node->point;




    return ret_val;
}

/**
* Recursive helper function for FindNearestNeighbour which traverses the recursively
* while checking if it should go to left child or right child.
**/

template <int Dim>
typename KDTree<Dim>::KDTreeNode* KDTree<Dim>::walker( KDTreeNode* current, Point<Dim> target, int dimension, int &best_distance_sqrd ,KDTreeNode*& best_node)const{

  if(current==NULL){
    return best_node;
  }
  Point<Dim> current_point = current->point;
  int nextdimension= (dimension+1)%Dim;

  KDTreeNode * other;
  if(target[dimension]<current_point[dimension] )
    { walker(current->left, target, nextdimension,best_distance_sqrd,best_node);
      other=current->right;
    }
  else if(current_point[dimension]<target[dimension]){
    walker(current->right,target, nextdimension,best_distance_sqrd,best_node);
    other=current->left;
  }
  else{
    if(target<current_point )
      {walker(current->left,target, nextdimension,best_distance_sqrd,best_node);
        other=current->right;
      }
    else {
      walker(current->right,target, nextdimension,best_distance_sqrd,best_node);
      other=current->left;
    }
  }


  int dissqr =0;
  for(int i=0;i<Dim;i++){
    dissqr += (target[i]-current_point[i])*(target[i]-current_point[i]);
  }
  if (dissqr<best_distance_sqrd){

    best_distance_sqrd = dissqr;
    best_node = current;

  }
  else if(dissqr==best_distance_sqrd){

    if (current_point<best_node->point){

      best_node = current;
    }
  }

  if(((target[dimension]-current_point[dimension])*(target[dimension]-current_point[dimension]))<=best_distance_sqrd){
    walker(other,target, nextdimension,best_distance_sqrd,best_node);
  }

  return best_node;
}
