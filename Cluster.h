/*

dependencies:
Base.h

FunSurf.h  - so here, we need a basic prototype/iface of org.

Link.h --> problem is depends on Org.h
Node.h
Cluster.h
Org.h
      Lugar.h
Pop.h

Stack.h
TrainingSets.h
*/

//#pragma once
#ifndef CLUSTER_H_INCLUDED
#define CLUSTER_H_INCLUDED

#include "Node.h"

/* ********************************************************************** */
//class NodeVec;
//class OrgPtr;
class Cluster;
typedef Cluster *ClusterPtr;
typedef std::vector<ClusterPtr> ClusterVec;
class Cluster {
public:
  NodeKit<>::NodeVec *NodeListPtr;
  NodeKit<>::NodeVec NodeList;
  derived<int> deriv;

//  Node<int>::NodeVec *NodeListPtr;
//  Node<int>::NodeVec NodeList;
  /*
Need a way to connect some nodes to mirror cluster.
Not just normal connections, but for comparison and scoring.
Could just make it 0 to N for both.

What is the testing process?
mirror fires all. main fires all. all main IO nodes compare value with mirror value. difference(?) goes to metanet.
or, just mirror value goes to metanet. so every metanet (on link) can read value being transmitted.

ok, so every metanet can read the raw fire passing through its own link.
either just the mirror's node fire is copied back to the trainer net, or
each trainer node has a seperate corrector/rawmirrorfire port, where it receives the value and passes it ONLY to the metanets. con:extra structural overhead.

ok, so every metanet can simply be connected directly to every other metanet, at compile time.
every metanet rides, reads and controls one inlink.

is a listening post an array of nodes? each receives a metanet link without weight (1.0).

a listening post:
receives and sums inputs from outer firing nodes.
allows other nodes to read those sums.
an IO node sums its own input, then exchanges its fire with the fire of another.

  */
  /* ********************************************************************** */
  Cluster() {
    NodeListPtr = &(NodeList);
  }
  /* ********************************************************************** */
  Cluster(int Num_Nodes) : Cluster() {
    NodeListPtr = &(NodeList);
    Fill_With_Nodes(Num_Nodes);
  }
  /* ********************************************************************** */
  ~Cluster() {
    NodeKit<>::NodePtr ndp;
    for (int ncnt=0; ncnt<this->NodeList.size(); ncnt++) {
      ndp = this->NodeList.at(ncnt);
      delete ndp;
    }
  }
  /* ********************************************************************** */
  void Fill_With_Nodes(int Num_Nodes) {
    NodeKit<>::NodePtr ndp;
    int cnt;
    this->NodeList.resize(Num_Nodes);
    for (cnt=0; cnt<Num_Nodes; cnt++) {
      ndp = new Node<>();
      this->NodeList.at(cnt) = ndp;
    }
  }
  /* ********************************************************************** */
  void Connect_Self() {
    this->Connect_Other_Cluster(this);
  }
  /* ********************************************************************** */
  void Connect_Other_Cluster(ClusterPtr upstreamer) {
    // Connect upstream cluster all-to-all to this one's inputs
    size_t siz0 = this->NodeList.size();
    size_t siz1 = upstreamer->NodeList.size();
    NodeKit<>::NodePtr dsn, usn;
    int cnt0, cnt1;
    for (cnt0=0; cnt0<siz0; cnt0++) {
      dsn = this->NodeList.at(cnt0);
      for (cnt1=0; cnt1<siz1; cnt1++) {
        usn = upstreamer->NodeList.at(cnt1);
        dsn->ConnectIn(usn);
      }
    }
  }
  /* ********************************************************************** */
  void Print_Specs(){}
  /* ********************************************************************** */
  void Set_Learning_Rate(double LRate) {
    NodeKit<>::NodePtr ndp;
    for (int ncnt=0; ncnt<this->NodeList.size(); ncnt++) {
      ndp = this->NodeList.at(ncnt);
      ndp->LRate = LRate;
    }
  }
  /* ********************************************************************** */
  void Attach_FunSurf(OrgProtoPtr fsurf0) {
    NodeKit<>::NodePtr ndp;
    for (int ncnt=0; ncnt<this->NodeList.size(); ncnt++) {
      ndp = this->NodeList.at(ncnt);
      ndp->Attach_FunSurf(fsurf0);
    }
  }
  /* ********************************************************************** */
  void Push_Fire() {
    NodeKit<>::NodePtr ndp;
    int cnt;
    size_t siz = this->NodeList.size();
    for (cnt=0; cnt<siz; cnt++) {
      ndp = this->NodeList.at(cnt);
      ndp->Push_Fire();
    }
    // right here the fire vals reside in the links
  }
  /* ********************************************************************** */
  void Fetch_Nodes(NodeKit<>::NodeVec *nvec) {
    NodeKit<>::NodePtr ndp;
    int cnt;
    size_t siz = this->NodeList.size();
    for (cnt=0; cnt<siz; cnt++) {
      ndp = this->NodeList.at(cnt);
      nvec->push_back(ndp);
    }
  }
  /* ********************************************************************** */
  void Fetch_Links(LinkVec *lvec) {
    NodeKit<>::NodePtr ndp;
    int cnt;
    size_t siz = this->NodeList.size();
    for (cnt=0; cnt<siz; cnt++) {
      ndp = this->NodeList.at(cnt);
      ndp->Fetch_Links(lvec);
    }
  }
  /* ********************************************************************** */
  void Collect_And_Fire() {
    NodeKit<>::NodePtr ndp;
    int cnt;
    // right here the fire vals reside in the links
    size_t siz = this->NodeList.size();
    for (cnt=0; cnt<siz; cnt++) {
      ndp = this->NodeList.at(cnt);
      ndp->Collect_And_Fire();
    }
  }
  /* ********************************************************************** */
  void Fire_Gen() {
    Push_Fire();
    // right here the fire vals reside in the links
    Collect_And_Fire();
  }
  /* ********************************************************************** */
  void Randomize_Weights() {
    NodeKit<>::NodePtr ndp;
    size_t cnt;
    size_t siz = this->NodeList.size();
    for (cnt=0; cnt<siz; cnt++) {
      ndp = this->NodeList.at(cnt);
      ndp->Randomize_Weights();
    }
  }
  /* ********************************************************************** */
  void Adapt_Weights() {
    NodeKit<>::NodePtr ndp;
    size_t cnt;
    size_t siz = this->NodeList.size();
    for (cnt=0; cnt<siz; cnt++) {
      ndp = this->NodeList.at(cnt);
      ndp->Adapt_Weights();
    }
  }
  /* ********************************************************************** */
  void Print_Me(int ClusterNum) {
    size_t cnt;
    NodeKit<>::NodePtr ndp;
    printf(" --------------------------------\n");
    printf(" Cluster ClusterNum:%li, this:%p, ", ClusterNum, this);
    size_t siz = this->NodeList.size();
    printf(" num nodes:%li\n", siz);
    for (cnt=0; cnt<siz; cnt++) {
      ndp = this->NodeList.at(cnt);
      ndp->Print_Me();
    }
    printf("\n");
  }
  /* ********************************************************************** */
  void Load_Inputs(std::vector<double> *invec) {
    size_t siz = invec->size();
    if (siz<this->NodeList.size()) { siz = this->NodeList.size(); }
    for (size_t cnt=0; cnt<siz; cnt++) {
      this->NodeList.at(cnt)->FireVal = invec->at(cnt);
    }
  }
};

#endif // CLUSTER_H_INCLUDED
