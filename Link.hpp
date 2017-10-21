#ifndef LINK_HPP_INCLUDED
#define LINK_HPP_INCLUDED

#include "Org.hpp"

#define LinkOrg 1

#define WeightAmp 2.0;

class Node; // forward
typedef Node *NodePtr;
/* ********************************************************************** */
class Link;// forward
typedef Link *LinkPtr;
typedef std::vector<LinkPtr> LinkVec;
class Link {
public:
  int numins=Org::DefaultWdt;//, Org::DefaultHgt
  VectPtr state;// These is the pie I put on my windowsill for Node to eat and leave a tip.
  NodePtr USNode, DSNode;
  #if LinkOrg
  int Num_Matrix_Iterations = 3;
  MatrixPtr genome;
  #endif // LinkOrg
  /* ********************************************************************** */
  Link() {
    state = new Vect(this->numins);
    state->Fill(1.0);
  }
  /* ********************************************************************** */
  ~Link() {
    delete state;
  }
  /* ********************************************************************** */
  void Attach_Genome(MatrixPtr genome0) {
    this->genome=genome0;
  }
  /* ********************************************************************** */
  void Clear_State() {
    this->state->Fill(1.0);
  }
  #if LinkOrg
  /* ********************************************************************** */
  void Run_Org() {
    genome->Iterate(state, Num_Matrix_Iterations, state);// using the same vector for input and output
  }
  #endif // LinkOrg
  /* ********************************************************************** */
  void DeState() {// remove all stateful information from org/matrix, but keep weight
    state->Fill(0.0);
  }
};


#endif // LINK_HPP_INCLUDED
