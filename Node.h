#pragma once
#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

/* ********************************************************************** */
/*  THIS IS BACKPROP NODE */
/* ********************************************************************** */

#include <iostream>
#include <stdio.h> // printf
#include <map>
#include <list>
#include "Base.h"
#include "FunSurf.h"
#include "Link.h"
#include "OrgProto.h"

/*
need to define 3 node classes:

base class, can connect, fire, etc.
Node_Micro: mutates, plugs in to meeting posts, doesn't own any meeting posts.
Node_Macro: never mutates, owns a meeting post, does not plug in to anything (except Entorno nodes maybe, but no flag needed)

Node_Mega?

when an org is created, it creates and owns MicroNodes.
when Entorno and ClayNet are created, they create and own MegaNodes.
therefore, different create/populate fns for each.

also 2 link classes?
MegaLink: owns an org
MicroLink: does not own an org

problem with 2 types of links, node base class can't have its own lists of links, UNLESS the base lists are just an overridable mask for native lists


ok, starting with 1 type of node and 1 type of link
both have an 'activate me' set of recursives, and an 'activate org below' set of recursives.

mega nodes and links would have 'activate org below' while micros would not.

orga do have mutation, and their micros would too.  megas would not.

easiest just to give everyone mutation.


also, there would be 2 types of clusters (mega or org), 2 types of nodes, and 2 types of links.

meta-calls from the top would not be a problem if the top is all meta types

*/


#define WeightAmp 2.0;

//namespace IoType {
//  enum IoType {Intra=0, GlobalIO=1, NbrIO=2};
//}
//typedef std::vector<NodePtr> NodeVec;
/* ********************************************************************** */
template <typename GottaBeALink>
class Node {
public:
  LinkVec Working_Ins, Working_Outs;
  double RawFire, FireVal, PrevFire;
  //Registry<Employee>::Type employeeRoster;
  typename NodeKit<GottaBeALink>::NodePtr other2;
  //typedef Node<GottaBeALink> *NodePtr;
  //typedef std::vector<NodeKit<>::NodePtr> NodeVec;
  /* ********************************************************************** */
  Node() {
    Init();
    this->LRate = 0.0;
  }
  /* ********************************************************************** */
  ~Node() {
    int cnt;
    for (cnt=0; cnt<this->Working_Ins.size(); cnt++) {
      delete this->Working_Ins.at(cnt);
    }
    this->Working_Ins.clear();// probably not necessary
    this->Working_Outs.clear();
  }
  /* ********************************************************************** */
  void Init() {
    this->FireVal = ((frand()*2.0)-1.0)*0.001;
    this->PrevFire = ((frand()*2.0)-1.0)*0.001;
    MinCorr = INT32_MAX;
    MaxCorr = INT32_MIN;
  }
  /* ********************************************************************** */
  void Collect_And_Fire() {
    LinkPtr ups;
    double Sum=0;
    size_t siz = this->Working_Ins.size();
    for (int cnt=0; cnt<siz; cnt++) {
      ups = this->Working_Ins.at(cnt);
      Sum+=ups->GetFire();
    }
    this->RawFire = Sum;
    this->FireVal = ActFun(Sum);
  }
  /* ********************************************************************** */
  double ActFun(double xin) {
    double OutVal;
    OutVal = xin / sqrt(1.0 + xin * xin);/* symmetrical sigmoid function in range -1.0 to 1.0. */
    return OutVal;
    /* General formula: double power = 2.0; OutVal = xin / Math.pow(1 + Math.abs(Math.pow(xin, power)), 1.0 / power); */
  }
  /* ********************************************************************** */
  void Push_Fire() {
    LinkPtr downs;
    double MyFire=this->FireVal;
    size_t siz = this->Working_Outs.size();
    for (int cnt=0; cnt<siz; cnt++) {
      downs = this->Working_Outs.at(cnt);
      downs->FireVal = MyFire;
    }
  }
  /* ********************************************************************** */
  void Fetch_Links(LinkVec *lvec) {
    LinkPtr ups;
    double MyFire=this->FireVal;
    size_t siz = this->Working_Outs.size();
    for (int cnt=0; cnt<siz; cnt++) {
      ups = this->Working_Ins.at(cnt);
      lvec->push_back(ups);
    }
  }
  /* ********************************************************************** */
  void Print_Me() {
    printf(" Node FireVal:%lf, MinCorr:%lf, MaxCorr:%lf, LRate:%lf, this:%p, ", this->FireVal, MinCorr, MaxCorr, LRate, this);
    size_t siz = this->Working_Ins.size();
    printf(" numlinks:%li\n", siz);
    return;// snox
    for (int cnt=0; cnt<siz; cnt++) {
      LinkPtr lnk = this->Working_Ins.at(cnt);
      lnk->Print_Me();
    }
  }
  /* ********************************************************************** */
  void ConnectIn(typename NodeKit<GottaBeALink>::NodePtr other) {// attach upstream node to me
    LinkPtr ln = new Link();
    ConnectIn(other, ln);
  }
  /* ********************************************************************** */
  void Randomize_Weights() {
    LinkPtr ups;
    Init();// clear metrics, etc.
    size_t siz = this->Working_Ins.size();
    for (int cnt=0; cnt<siz; cnt++) {
      ups = this->Working_Ins.at(cnt);
      ups->Randomize_Weight();
    }
  }
  /* ********************************************************************** */
  void ConnectIn(typename NodeKit<GottaBeALink>::NodePtr other, LinkPtr ln) {// attach upstream node to me
    this->Working_Ins.push_back(ln);// this approach uses less memory, fewer allocations/frees and is probably faster.
    other->Working_Outs.push_back(ln);
    ln->USNode = other; ln->DSNode = this;
  }
//class Sigmoid {   public:
  /* *************************************************************************************************** */
  static double sigmoid_deriv_raw(double Value) {
    double vsq = Value*Value;// pre sym sigmoid deriv (from raw sum before actfun) genuine derivative
    double denom1 = pow((vsq + 1.0),(3/2));
    double retval = ( 1.0/sqrt(vsq + 1.0) ) - ( vsq / denom1 );
    return retval;
    /*
    http://www.quickmath.com/webMathematica3/quickmath/calculus/differentiate/basic.jsp#c=differentiate_basicdifferentiate&v1=%28x+%2F+sqrt%281.0+%2B+x*x%29%29&v2=x
    pre sym sigmoid deriv (from raw sum before actfun):
    ( 1.0/sqrt(x*x + 1.0) ) - ( x*x / (x*x + 1.0)^(3/2) )
    */
  }
  /* *************************************************************************************************** */
  static double sigmoid_deriv_postfire(double Value) {
    double MovedValue = (1.0+Value)/2.0;// first map range -1 ... +1 to 0 ... +1
    double retval = 2.0 * MovedValue * (1.0-MovedValue);// APPROXIMATE post sym sigmoid deriv (from fire value after actfun):
    return retval;
    /*
    APPROXIMATE post sym sigmoid deriv (from fire value after actfun):
    4*(x+0.5) * (1.0-(x+0.5))

    output * (1 - output)  derivative of ASYM sigmoid function, after actfun (hillock function)
    */
  }
//};
  /* ******************************************************************************************************************************************** */
  /* ******************************************************************************************************************************************** */
#if 1 // Mega stuff
  /* ********************************************************************** */
  typedef std::vector<double> MeetingPost;
  MeetingPost MPost;
  double LRate;
  double MinCorr, MaxCorr;
  /* ********************************************************************** */
  void Attach_FunSurf(OrgProtoPtr fsurf0) {
    LinkPtr lnk;
    int cnt;
    for (cnt=0; cnt<this->Working_Ins.size(); cnt++) {
      lnk = this->Working_Ins.at(cnt);
      lnk->Attach_FunSurf(fsurf0);
      fsurf0->Attach_Link(lnk);
    }
  }
  /* ********************************************************************** */
  void Adapt_Weights() {
    LinkPtr ups;
    Init();// clear metrics, etc.
    size_t siz = this->Working_Ins.size();
    for (int cnt=0; cnt<siz; cnt++) {
      ups = this->Working_Ins.at(cnt);
      ups->Adapt_Weight();
    }
  }
  /* ********************************************************************** */
  void Fetch_SubNodes(NodeKit<>::NodeVec *nvec) {
    LinkPtr ups;
    size_t siz = this->Working_Ins.size();
    for (int cnt=0; cnt<siz; cnt++) {
      ups = this->Working_Ins.at(cnt);
      ups->Fetch_SubNodes(nvec);
    }
  }
  /* ********************************************************************** */
  void Fetch_SubLinks(LinkVec *lvec) {
    LinkPtr ups;
    size_t siz = this->Working_Ins.size();
    for (int cnt=0; cnt<siz; cnt++) {
      ups = this->Working_Ins.at(cnt);
      ups->Fetch_SubLinks(lvec);
    }
  }
#endif

  /* ******************************************************************************************************************************************** */
  /* ******************************************************************************************************************************************** */
#if 1 // Micro stuff
  LinkVec LGenome;
  /* ********************************************************************** */
  NodePtr Spawn() {
    NodePtr child;
    child = new Node();
    size_t siz = this->LGenome.size();
    LinkPtr lnkptr;
    child->LGenome.resize(siz);
    for (int cnt=0; cnt<siz; cnt++) {
      lnkptr = this->LGenome.at(cnt);
      lnkptr = lnkptr->Spawn();
      child->LGenome.at(cnt) = lnkptr;
    }
    return child;
  }
#endif
};

namespace IoType {
  enum IoType {UStream=0, DStream=1, Link=2};
}
/* ********************************************************************** */
class Node_Micro : public Node<> {
public:
  IoType::IoType My_Type;
  void Mutate_Me() {
  }
};
/* ********************************************************************** */
class Node_Mega : public Node<> {
};


/* ******************************************************************************************************************************************** */
//  Test of circular reference between two template classes
/* ******************************************************************************************************************************************** */
/* ********************************************************************** */
template <typename YType>
class YinBase {
};
/* ********************************************************************** */
template <typename YType>
class YangBase {
};
/* ********************************************************************** */
class YangDeriv;// forward
class YinDeriv : public YinBase<YangDeriv> {
};
/* ********************************************************************** */
class YangDeriv : public YangBase<YinDeriv> {
};

#endif // NODE_H_INCLUDED


