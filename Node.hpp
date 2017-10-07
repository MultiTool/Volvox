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
  const static int VecTradeSize = 3;
  VectPtr CrossRoads;// meeting place for all links
  #ifndef LinkOrg
  int Num_Matrix_Iterations = 3;
  MatrixPtr genome;
  const static int CrossRoadsSize = VecTradeSize;
  #else
  const static int CrossRoadsSize = VecTradeSize+3;
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
  void Link_Apply(std::function<void(LinkPtr)> LinkFunc) {// apply whatever function to every link
    LinkPtr ups;
    size_t siz = this->InLinks.size();
    for (size_t cnt=0; cnt<siz; cnt++) {
      ups = this->InLinks.at(cnt);
      LinkFunc(ups);
    }
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
    #ifdef LinkOrg
    this->CrossRoads->Fill(FireVal);
    #else
    this->CrossRoads->Fill(FireVal, VecTradeSize);
    #endif // LinkOrg
  }
  /* ********************************************************************** */
  void Collect_And_Fire() {
    LinkPtr ups;
    size_t siz;
    Run_Orgs();// get all my inlinks to compute their output vectors
    this->CrossRoads->Fill(0.0);// clear
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
      //downs->Run_Org();// could alternatively force outlinks to run, and update their own output vectors
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
