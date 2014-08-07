#ifndef LINK_H_INCLUDED
#define LINK_H_INCLUDED

#include <iostream>
#include <stdio.h> // printf
#include <map>
#include <list>
#include "Base.h"
#include "FunSurf.h"
#include "Node.h"
#include "OrgProto.h"

#define WeightAmp 2.0;

class Node;
typedef Node *NodePtr;
/* ********************************************************************** */
class Link;
typedef Link *LinkPtr;
class Link {//} : public LinkProto {;
public:
  typedef double WeightType;
  WeightType Weight; double FireVal;
  NodePtr USNode,DSNode;
  OrgProtoPtr My_Org;
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
  void Adapt_Weight() {
    My_Org->Run_Cycle();
  }
  /* ********************************************************************** */
  void Attach_FunSurf(OrgProtoPtr Org0) {
    this->My_Org = Org0;
    Org0->Attach_Link(this);
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
