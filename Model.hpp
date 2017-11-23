#ifndef MODEL_HPP_INCLUDED
#define MODEL_HPP_INCLUDED

#include "Matrix.hpp"

/* ********************************************************************** */
class Model;// forward
typedef Model *ModelPtr;
typedef std::vector<ModelPtr> ModelVec;
class Model: public Matrix {
public:
  VectPtr StartingState;
  /* ********************************************************************** */
  Model(int wdt0, int hgt0) : Matrix(wdt0, hgt0){
    StartingState = new Vect(wdt0);
  }
  /* ********************************************************************** */
  ~Model() {
    delete StartingState;
  }
  /* ********************************************************************** */
  void Assign_StateSeed(VectPtr source){
    this->StartingState->Copy_From(source);
  }
  /* ********************************************************************** */
  void Assign_Matrix(MatrixPtr source){
    this->Copy_From(source);
  }
};

#endif // MODEL_HPP_INCLUDED

