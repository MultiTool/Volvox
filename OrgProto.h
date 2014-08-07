#ifndef ORGPROTO_H_INCLUDED
#define ORGPROTO_H_INCLUDED

#include "Base.h"

//class Node;
//typedef Node *NodePtr;
/* ********************************************************************** */
//class LinkProto;
//typedef LinkProto *LinkProtoPtr;
//class LinkProto {
//public:
//  typedef double WeightType;
//  WeightType Weight;
//  double FireVal;
//};
/* ********************************************************************** */
class Link;
typedef Link *LinkPtr;

class OrgProto;
typedef OrgProto *OrgProtoPtr;
typedef std::vector<OrgProtoPtr> OrgProtoVec;
class OrgProto {// This class is like an interface in Java, a contract with no real content.
public:
  /* ********************************************************************** */
  OrgProto() { }
  /* ********************************************************************** */
  ~OrgProto() { }
  /* ********************************************************************** */
  static OrgProtoPtr Abiogenate() {
    OrgProtoPtr org = new OrgProto();
    return org;
  }
  /* ********************************************************************** */
  virtual OrgProtoPtr Spawn() {// May have to get rid of this
    OrgProtoPtr child;
    child = new OrgProto();
    return child;
  }
  /* ********************************************************************** */
  virtual void Print_Me() { }
  /* ********************************************************************** */
  virtual void Attach_Link(LinkPtr lnk) { }
  /* ********************************************************************** */
  virtual void Run_Cycle() { }
};

#endif // ORGPROTO_H_INCLUDED
