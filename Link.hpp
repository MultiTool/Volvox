#ifndef LINK_HPP_INCLUDED
#define LINK_HPP_INCLUDED

#include "Org.hpp"

#define LinkOrg 1 // toggles whether network organisms operate within the nodes or within the links

#define WeightAmp 2.0;

class Node; // forward
typedef Node *NodePtr;
/* ********************************************************************** */
class Link;// forward
typedef Link *LinkPtr;
typedef std::vector<LinkPtr> LinkVec;
class Link {
public:
  int numins=0;
  Vect state;// These is the pie I put on my windowsill for Node to eat and leave a tip.
  NodePtr USNode, DSNode;
  #if LinkOrg
  int Num_Matrix_Iterations = 5;
  MatrixPtr genome;
  #endif // LinkOrg
  /* ********************************************************************** */
  Link() {
    state.Resize(this->numins);
    Clear_State();
  }
  /* ********************************************************************** */
  ~Link() {//delete state;
  }
  /* ********************************************************************** */
  void Attach_Genome(MatrixPtr genome0) {
    this->genome=genome0;
    this->numins = genome0->wdt;
    state.Resize(this->numins);
  }
  /* ********************************************************************** */
  void Clear_State() {
    this->state.Fill(1.0);
  }
  #if LinkOrg
  /* ********************************************************************** */
  void Run_Org() {
    genome->Iterate(&state, Num_Matrix_Iterations, &state);// using the same vector for input and output
  }
  #endif // LinkOrg
  /* ********************************************************************** */
  void DeState() {// remove all stateful information from org/matrix, but keep weight
    state.Fill(0.0);
  }
};


#endif // LINK_HPP_INCLUDED
