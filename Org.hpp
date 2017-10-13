#ifndef ORG_HPP_INCLUDED
#define ORG_HPP_INCLUDED

#include "Matrix.hpp"

/* ********************************************************************** */
class Org;// forward
typedef Org *OrgPtr;
typedef std::vector<OrgPtr> OrgVec;
class Org: public Matrix {
public:
  static const int NumScores=2;
  double Score[NumScores];
  bool Doomed = false;
  //MatrixPtr matrix;
  static const int DefaultWdt = 5, DefaultHgt = 5;
  /* ********************************************************************** */
  Org(int wdt0, int hgt0) : Matrix(wdt0, hgt0) {
  }
  /* ********************************************************************** */
  static OrgPtr Abiogenate() {
    OrgPtr child = new Org(DefaultWdt, DefaultHgt);
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
//  double Compare(VectPtr other){// strictly for genalg scoring
//    int ln = std::min(this->len, other->len);
//    double range = 2.0;
//    double val0, val1, digival0, digival1, diff, digidiff, digiscore;
//    double singlescore, score = 1.0;
//    for (int cnt=0;cnt<ln;cnt++){
//      digival0 =  std::copysign(1.0, this->ray[cnt]);
//      digival1 =  std::copysign(1.0, other->ray[cnt]);
//      digidiff=std::fabs(digival0-digival1);
//      digiscore+=(range-digidiff)/range;
//      val0 =  this->ray[cnt];
//      val1 =  other->ray[cnt];
//      diff=std::fabs(val0-val1);
//      singlescore=(range-diff)/range;
//      score*=singlescore;
//    }
//    return score;
//  }
};

#endif // ORG_HPP_INCLUDED
