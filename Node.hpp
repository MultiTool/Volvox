#ifndef NODE_HPP_INCLUDED
#define NODE_HPP_INCLUDED

#include <functional>

#include "Link.hpp"

/* ********************************************************************** */
class Node;// forward
typedef Node *NodePtr;
typedef std::vector<NodePtr> NodeVec;
class Node {
public:
  LinkVec InLinks, OutLinks;
  const static int VecTradeSize = 3;//3;// This is the size of the signal that all neighboring links pool together.
  const static int ExternalCommSize = VecTradeSize;// size of the signal that is read/written by things external to this whole network. must be less than or equal to VecTradeSize.
  VectPtr CrossRoads;// meeting place for all links
  #ifdef LinkOrg
  const static int CrossRoadsSize = VecTradeSize;// if only links have processors, then 100% of CrossRoads is dedicated to just communication entre si.
  #else
  int Num_Matrix_Iterations = 3;
  MatrixPtr genome;
  const static int CrossRoadsSize = Org::DefaultWdt;// If the node is intelligent instead of the link, then it will need some extra numbers for internal processing.
  const static int NodeRecurrenceSize = CrossRoadsSize-VecTradeSize;// not used yet.
  #endif // LinkOrg
  /* ********************************************************************** */
  Node() {
    CrossRoads = new Vect(CrossRoadsSize);
  }
  /* ********************************************************************** */
  ~Node() {
    delete CrossRoads;
    for (size_t cnt=0; cnt<this->InLinks.size(); cnt++) {
      delete this->InLinks.at(cnt);
    }
    this->InLinks.clear();// clearing probably not necessary
    this->OutLinks.clear();
  }
  /* ********************************************************************** */
  void Attach_Genome(MatrixPtr genome0) {
    LinkPtr ups;
    size_t siz = this->InLinks.size();
    for (size_t cnt=0; cnt<siz; cnt++) {
      ups = this->InLinks.at(cnt);
      ups->Attach_Genome(genome0);
    }
  }
  /* ********************************************************************** */
  void Clear_State() {
    this->CrossRoads->Fill(1.0);
    LinkPtr ups;
    size_t siz = this->InLinks.size();
    for (size_t cnt=0; cnt<siz; cnt++) {
      ups = this->InLinks.at(cnt);
      ups->Clear_State();
    }
  }
  /* ********************************************************************** */
  void Link_Apply(std::function<void(LinkPtr)> LinkFunc) {// apply whatever function to every link
    LinkPtr ups;
    size_t siz = this->InLinks.size();
    for (size_t cnt=0; cnt<siz; cnt++) {
      ups = this->InLinks.at(cnt);
      LinkFunc(ups);
    }
  }
  /* ********************************************************************** */
  void Connect2Way(NodePtr other) {// attach upstream node to me
    this->ConnectIn(other);
    other->ConnectIn(this);
  }
  /* ********************************************************************** */
  void ConnectIn(NodePtr other) {// attach upstream node to me
    LinkPtr ln = new Link();
    ConnectIn(other, ln);
  }
  /* ********************************************************************** */
  void ConnectIn(NodePtr other, LinkPtr ln) {// attach upstream node to me
    this->InLinks.push_back(ln);// this approach uses less memory, fewer allocations/frees and is probably faster.
    other->OutLinks.push_back(ln);
    ln->USNode = other; ln->DSNode = this;// probably not needed
  }
  /* ********************************************************************** */
  void Load_Input(double FireVal) {
    this->CrossRoads->Fill(FireVal, ExternalCommSize);// only use a subset of crossroads vector for external communication, and the rest for internal comm or recurrence.
  }
  /* ********************************************************************** */
  double Get_Output() {
    double OutVal=0;
    for (int cnt=0;cnt<ExternalCommSize;cnt++){
      OutVal += this->CrossRoads->ray.at(cnt);
    }
    double result = OutVal/ExternalCommSize;// average
    return ActFun(OutVal);
  }
  /* ********************************************************************** */
  void Collect_And_Fire() {
    LinkPtr ups;
    size_t siz;
    Run_Orgs();// get all my inlinks to compute their output vectors
    this->CrossRoads->Fill(0.0, VecTradeSize);// clear all shared channels, but not recurrent ones, if any
    siz = this->InLinks.size();
    for (size_t cnt=0; cnt<siz; cnt++) {
      ups = this->InLinks.at(cnt);// read link output vector here, and add it to my own
      this->CrossRoads->Add_To_Me(ups->state, VecTradeSize);
    }
  }
  /* ********************************************************************** */
  void Push_Fire() {
    LinkPtr downs;
    size_t siz;
    siz = this->OutLinks.size();
    for (size_t cnt=0; cnt<siz; cnt++) {
      downs = this->OutLinks.at(cnt);// after summing/processing the node vector, push it out to the input vectors of all links
      downs->state->Copy_From(this->CrossRoads, VecTradeSize);
    }
  }
  /* ********************************************************************** */
  void Run_Orgs() {
    LinkPtr ups;
    #ifdef LinkOrg
    size_t siz = this->InLinks.size();
    for (size_t cnt=0; cnt<siz; cnt++) {
      ups = this->InLinks.at(cnt);
      ups->Run_Org();
    }
    #else
    genome->Iterate(this->CrossRoads, Num_Matrix_Iterations, this->CrossRoads);// using the same vector for input and output
    #endif // LinkOrg
  }
};

#endif // NODE_HPP_INCLUDED
