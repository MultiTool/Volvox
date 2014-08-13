#ifndef ORGPROTO_H_INCLUDED
#define ORGPROTO_H_INCLUDED

#include "Base.h"

/* ********************************************************************** */
class OrgProto;
typedef OrgProto *OrgProtoPtr;
typedef std::vector<OrgProtoPtr> OrgProtoVec;
class OrgProto {// This class is like a Java interface, a contract with no real content.
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
  /* ********************************************************************** */
  virtual void OrgProto_Fetch_SubNodes(NodeKit<>::NodeVec *nvec) {}// = 0;
  /* ********************************************************************** */
  virtual void OrgProto_Fetch_SubLinks(LinkVec *lvec) {}// = 0;
};

#endif // ORGPROTO_H_INCLUDED
