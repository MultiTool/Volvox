#ifndef ORGPROTO_H_INCLUDED
#define ORGPROTO_H_INCLUDED

#include "Base.h"

/* ********************************************************************** */
class OrgProto;
typedef OrgProto *OrgProtoPtr;
typedef std::vector<OrgProtoPtr> OrgProtoVec;
class OrgProto {
public:
  /* ********************************************************************** */
  OrgProto() {
  }
  /* ********************************************************************** */
  ~OrgProto() {
  }
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
};

#endif // ORGPROTO_H_INCLUDED
