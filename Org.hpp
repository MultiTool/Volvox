#ifndef ORG_HPP_INCLUDED
#define ORG_HPP_INCLUDED

#include "Matrix.hpp"

/* ********************************************************************** */
class Org;// forward
typedef Org *OrgPtr;
typedef std::vector<OrgPtr> OrgVec;
class Org: public Matrix {
public:
  static const int NumScores=3;
  double Score[NumScores]={};
  bool Doomed = false;
  double ModelStateMag;
  //MatrixPtr matrix;
  //static const int DefaultWdt = 2, DefaultHgt = 2;
  //static const int DefaultWdt = 3, DefaultHgt = 3;
  static const int DefaultWdt = 4, DefaultHgt = 4;
  //static const int DefaultWdt = 5, DefaultHgt = 5;
  //static const int DefaultWdt = 6, DefaultHgt = 6;
  //static const int DefaultWdt = 8, DefaultHgt = 8;
  //static const int DefaultWdt = 16, DefaultHgt = 16;
  //static const int DefaultWdt = 18, DefaultHgt = 18;
  //static const int DefaultWdt = 32, DefaultHgt = 32;
  //static const int DefaultWdt = 64, DefaultHgt = 64;
  /* ********************************************************************** */
  Org(int wdt0, int hgt0) : Matrix(wdt0, hgt0) {
  }
  /* ********************************************************************** */
  static OrgPtr Abiogenate(size_t Wdt, size_t Hgt) {
    OrgPtr child = new Org(Wdt, Hgt);
    child->Rand_Init();
    return child;
  }
  /* ********************************************************************** */
  OrgPtr Spawn() {
    OrgPtr child = new Org(this->wdt, this->hgt);
    child->Copy_From(this);
    return child;
  }
  /* ********************************************************************** */
  void Reset() {
    Doomed = false; ModelStateMag = 0.0;
    for (int cnt=0;cnt<NumScores;cnt++){
      this->Score[NumScores] = 0;
    }
  }
  /* ********************************************************************** */
//  void Rand_Init(){
//    this->m>Rand_Init();// mutate 100%
//  }
  /* ********************************************************************** */
//  void Mutate_Me(double MRate) {// use this if matrix is a property of org
//    this->Mutate_Me(MRate);
//  }
  /* ********************************************************************** */
  int Compare_Score(OrgPtr other) {
    int cnt = 0;
    double *ScoreMe, *ScoreYou;
    ScoreMe=this->Score; ScoreYou=other->Score;
    while (cnt<NumScores) {
      if (ScoreMe[cnt]<ScoreYou[cnt]) {return 1;}
      if (ScoreMe[cnt]>ScoreYou[cnt]) {return -1;}
      cnt++;
    }
    return 0;
  }
  /* ********************************************************************** */
  void Print_Scores() {
    for (int cnt=0;cnt<NumScores;cnt++){
      printf("Sc%i:%1.10f, ", cnt, this->Score[cnt]);
    }
  }
};

#endif // ORG_HPP_INCLUDED
