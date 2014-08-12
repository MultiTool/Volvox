#ifndef LINK_H_INCLUDED
#define LINK_H_INCLUDED

#include <iostream>
#include <stdio.h> // printf
#include <map>
#include <list>
#include "Base.h"
#include "Forwards.h"
#include "FunSurf.h"
#include "Node.h"
#include "OrgProto.h"

#define WeightAmp 2.0;

/* ********************************************************************** */
//template <typename GottaBeANode>
class Link {
public:
  typedef double WeightType;
  WeightType Weight; double FireVal;
  NodeKit<Link>::NodePtr USNode,DSNode;
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
  void Attach_FunSurf2(Org *Org0) {
  }
  /* ********************************************************************** */
  void Print_Me() {
    printf("  Link ");
    printf("USNode:%p, DSNode:%p ", this->USNode, this->DSNode);
    printf("Weight:%lf \n", this->Weight);
  }
};
typedef std::vector<LinkPtr> LinkVec;


/* ********************************************************************** */
class Link_Micro : public Link {
public:
  OrgProtoPtr My_Org;
  /* ********************************************************************** */
  Link_Micro() : Link() {
    this->FireVal=0.0;
    this->USNode=NULL; this->DSNode=NULL;
    Randomize_Weight();
  }
  ~Link_Micro() {
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
  void Attach_FunSurf2(Org *Org0) {
  }
  /* ********************************************************************** */
  void Print_Me() {
    printf("  Link_Micro ");
    printf("USNode:%p, DSNode:%p ", this->USNode, this->DSNode);
    printf("Weight:%lf \n", this->Weight);
  }
};

#endif // LINK_H_INCLUDED
