#ifndef LINK_H_INCLUDED
#define LINK_H_INCLUDED

#include <iostream>
#include <stdio.h> // printf
#include <map>
#include <list>
#include "Base.h"
#include "FunSurf.h"
#include "Node.h"

#define WeightAmp 2.0;
typedef double WeightType;

class Node;
typedef Node *NodePtr;
/* ********************************************************************** */
class Link;
typedef Link *LinkPtr;
class Link {
public:
  WeightType Weight;
  double FireVal;
  NodePtr USNode,DSNode;
  FunSurfGridPtr fsurf;
  /* ********************************************************************** */
  Link() {
    this->FireVal=0.0;
    this->USNode=NULL; this->DSNode=NULL;
    Randomize_Weight();
  }
  ~Link() {
  }
  /* ********************************************************************** */
  inline double GetFire() {
    return this->FireVal*this->Weight;
  }
  void Randomize_Weight(){
    this->Weight = (frand()-0.5) * WeightAmp;// to do: do this with a distribution change
  }
  /* ********************************************************************** */
  void Attach_FunSurf(FunSurfGridPtr fsurf0) {
    this->fsurf = fsurf0;
  }
  /* ********************************************************************** */
  void Print_Me() {
    printf("  Link ");
    printf("USNode:%p, DSNode:%p ", this->USNode, this->DSNode);
    printf("Weight:%lf \n", this->Weight);
  }
};
typedef std::vector<LinkPtr> LinkVec;

#endif // LINK_H_INCLUDED
