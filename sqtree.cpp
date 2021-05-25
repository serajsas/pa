/**
 *
 * shifty quadtree (pa3)
 * sqtree.cpp
 * This file will be used for grading.
 *
 */

#include "sqtree.h"

// First Node constructor, given.
SQtree::Node::Node(pair<int,int> ul, int w, int h, RGBAPixel a, double v)
  :upLeft(ul),width(w),height(h),avg(a),var(v),NW(NULL),NE(NULL),SE(NULL),SW(NULL)
{}

// Second Node constructor, given
SQtree::Node::Node(stats & s, pair<int,int> ul, int w, int h)
  :upLeft(ul),width(w),height(h),NW(NULL),NE(NULL),SE(NULL),SW(NULL) {
  avg = s.getAvg(ul,w,h);
  var = s.getVar(ul,w,h);
}

// SQtree destructor, given.
SQtree::~SQtree() {
  clear();
}

// SQtree copy constructor, given.
SQtree::SQtree(const SQtree & other) {
  copy(other);
}

// SQtree assignment operator, given.
SQtree & SQtree::operator=(const SQtree & rhs) {
  if (this != &rhs) {
    clear();
    copy(rhs);
  }
  return *this;
}

/**
 * SQtree constructor given tolerance for variance.
 */
SQtree::SQtree(PNG & imIn, double tol) {
  int width = imIn.width();
  int height = imIn.height();
  stats stat(imIn);
  pair<int,int> first(0,0);
  root = buildTree(stat,first,width,height,tol);
}

/**
 * Helper for the SQtree constructor.
 */
SQtree::Node * SQtree::buildTree(stats & s, pair<int,int> & ul,
				 int w, int h, double tol) {
  // cout<<"BUILD TREE STARTING"<<endl;
  if(w==0||h==0) return NULL;
  Node* root_ = new Node(s,ul,w,h);
  if(tol>=root_->var||(w==1&&h==1)) return root_;
  double minVar = root_->var;
  double maxAmongPartitions;
  int x = ul.first;
  int y = ul.second;

  for(int i=ul.second;i<h+ul.second;i++){
    for(int j=ul.first;j<w+ul.first;j++){
      if(i==(ul.second+h-1) && j==(ul.first+w-1))break;
    
      
      double varNW = s.getVar(make_pair(ul.first,ul.second),j+1-ul.first,i+1-ul.second);
      maxAmongPartitions = varNW;
      double varNE = s.getVar(make_pair(j+1,ul.second),w-(j+1-ul.first),(i+1-ul.second));

      double varSE = s.getVar(make_pair(j+1,i+1),w-(j+1-ul.first),h-(i+1-ul.second));
 
      double varSW = s.getVar(make_pair(ul.first,i+1),j+1-ul.first,h-(i+1-ul.second));
   

      maxAmongPartitions = max(varNW,max(varNE,max(varSE,varSW)));
      if(maxAmongPartitions<=minVar){
        minVar = maxAmongPartitions;
        x = j+1;
        y = i+1;
      }
    }
  }

  pair<int,int> NW_pair(ul.first,ul.second);
  pair<int,int> NE_pair(x,ul.second);
  pair<int,int> SE_pair(x,y);
  pair<int,int> SW_pair(ul.first,y);

  root_->NW = buildTree(s,NW_pair,x-ul.first,y-ul.second,tol);

  root_->NE = buildTree(s,NE_pair,w-(x-ul.first),(y-ul.second),tol);

  root_->SW = buildTree(s,SW_pair,x-ul.first,h-(y-ul.second),tol);

  root_->SE = buildTree(s,SE_pair,w-(x-ul.first),h-(y-ul.second),tol);


  return root_;
}
  
/**
 * Render SQtree and return the resulting image.
 */
PNG SQtree::render() {
  // Your code here.
  PNG img = PNG(root->width,root->height);
  render_fn(&img,root);
  return img;

}

void SQtree::render_fn(PNG *img,Node* root_){
  if(!root_) return;
  if(!root_->NW&&!root_->NE&&!root_->SE&&!root_->SW){
    for(int i=root_->upLeft.second;i<root_->upLeft.second+root_->height;i++){
      for(int j=root_->upLeft.first;j<root_->upLeft.first+root_->width;j++){
        *(img->getPixel(j,i)) = root_->avg;
      }
    }
  }
  render_fn(img,root_->NW);
  render_fn(img,root_->NE);
  render_fn(img,root_->SE);
  render_fn(img,root_->SW); 
}


/**
 * Delete allocated memory.
 */
void SQtree::clear() {
  if (root==NULL)
    return;
  // clear_fn(root->NE);
  // clear_fn(root->NW);
  // clear_fn(root->SE);
  // clear_fn(root->SW);
  

  queue<Node *> node_q;
  node_q.push(root);
  while (!node_q.empty())
  {
    Node* temp = node_q.front();
    node_q.pop();

    if(temp->NW!=NULL){
      node_q.push(temp->NW);
    }
    if(temp->NE!=NULL){
      node_q.push(temp->NE);
    }
    if(temp->SE!=NULL){
      node_q.push(temp->SE);
    }
    if(temp->SW!=NULL){
      node_q.push(temp->SW);
    }
    delete temp;
    temp = NULL;
  }
}

void SQtree::clear_fn(Node* root_){
  if(root_==NULL) return;
  clear_fn(root_->NE);
  clear_fn(root_->NW);
  clear_fn(root_->SE);
  clear_fn(root_->SW);
  delete root_;
  root_ = NULL;
}

void SQtree::copy(const SQtree & other) {
  if(other.root==NULL) return;
  root = copy_func(other.root); 
}

SQtree::Node* SQtree::copy_func(Node* tree){
  if (tree == NULL)
        return NULL;
 
    Node* start = new Node(tree->upLeft,tree->width,tree->height,
                      tree->avg,tree->var);
    start->NW = copy_func(tree->NW);
    start->NE = copy_func(tree->NE);
    start->SE = copy_func(tree->SE);
    start->SW = copy_func(tree->SW);
    
  return start;
}

int SQtree::size() {
  int count = 0;
  if (root==NULL)
    return count;

  queue<Node *> node_q;
  node_q.push(root);
  count = 1;
  while (!node_q.empty())
  {
    Node* temp = node_q.front();
    node_q.pop();

    if(temp->NW!=NULL){
      node_q.push(temp->NW);
      count++;
    }
    if(temp->NE!=NULL){
      node_q.push(temp->NE);
      count++;
    }
    if(temp->SE!=NULL){
      node_q.push(temp->SE);
      count++;
    }
    if(temp->SW!=NULL){
      node_q.push(temp->SW);
      count++;
    }
  }
  return count;
}

