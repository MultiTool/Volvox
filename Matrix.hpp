#ifndef MATRIX_HPP_INCLUDED
#define MATRIX_HPP_INCLUDED

#include <iostream>
#include <cstdint>
#include <cstring>
#include <vector>

#include "Base.hpp"

/* ********************************************************************** */
class Vect;// forward
typedef Vect *VectPtr;
class Vect {
public:
  int len;
  //double* ray;// to do: change array to std vector
  std::vector<double> ray;
  //std::array<int, 3> a1{ {1, 2, 3} }; // double-braces required in C++11 (not in C++14)
  /* ********************************************************************** */
  Vect(int wdt0) {
    this->len = wdt0;
    ray.resize(wdt0, 0.01); //ray.reserve(wdt0);
    //double *array = &(*testvec.begin());

    //for (int cnt=0; cnt<this->len; cnt++) { this->ray[cnt]=0.01; }
  }
  /* ********************************************************************** */
  ~Vect() {
  }
  /* ********************************************************************** */
  void Copy_From(VectPtr source) {
    int ln = std::min(this->len,source->len);
    std::copy_n(source->ray.begin(), ln, this->ray.begin());
  }
  /* ********************************************************************** */
  void Copy_From(VectPtr source, int Limit) {
    int ln = std::min(std::min(this->len, source->len), Limit);
    std::copy_n(source->ray.begin(), ln, this->ray.begin());
  }
  /* ********************************************************************** */
  void Add_To_Me(VectPtr other, int Limit) {
    int ln = std::min(std::min(this->len, other->len), Limit);
    for (int cnt=0; cnt<ln; cnt++) {
      this->ray[cnt] += other->ray[cnt];
    }
  }
  /* ********************************************************************** */
  void Add(VectPtr other, VectPtr result) {
    int ln = std::min(this->len, other->len);
    for (int cnt=0; cnt<ln; cnt++) {
      result->ray[cnt] = this->ray[cnt]+other->ray[cnt];
    }
  }
  /* ********************************************************************** */
  double Mult(Vect* other) {
    int ln = std::min(this->len, other->len);
    double sum=0.0;
    for (int cnt=0; cnt<ln; cnt++) {
      sum+=this->ray[cnt]*other->ray[cnt];
    }
    return sum;
  }
  /* ********************************************************************** */
  double Magnitude() {
    int ln = this->len;
    double val, SumSq=0.0;
    for (int cnt=0; cnt<ln; cnt++) {
      val = this->ray[cnt];
      SumSq += val*val;
    }
    return std::sqrt(SumSq);
  }
  /* ********************************************************************** */
  double MultFire(Vect* other) {
    double Sum = this->Mult(other);
    return ActFun(Sum);
  }
  /* ********************************************************************** */
  void Fill(double val) {
    int ln = this->len;
    for (int cnt=0; cnt<ln; cnt++) {
      this->ray[cnt] = val;
    }
  }
  /* ********************************************************************** */
  void Fill(double val, int Limit) {
    int ln = std::min(this->len, Limit);
    for (int cnt=0; cnt<ln; cnt++) {
      this->ray[cnt] = val;
    }
  }
  /* ********************************************************************** */
  void Mutate_Me(double MRate) {
    int ln = this->len;
    double val, amp = 0.3;
    for (int cnt=0; cnt<ln; cnt++) {
      if (frand()<MRate) {
        val = this->ray[cnt];
        val += (frand()*2.0-1.0)*amp;
        //val = (frand()*2.0-1.0)*1.0;// amp = 1.0 here, range -1 to +1
        this->ray[cnt] = val;
      }
    }
  }
  /* ********************************************************************** */
  void Rand_Init() {
    int ln = this->len;
    double val;
    for (int cnt=0; cnt<ln; cnt++) {
      val = this->ray[cnt];
      val = (frand_nonzero()*2.0-1.0);// amp = 1.0 here, range -1 to +1
      //val = (frand()*2.0-1.0);// amp = 1.0 here, range -1 to +1
      this->ray[cnt] = val;
    }
  }
  /* ********************************************************************** */
  void Print_Me() {
    int ln = this->len;
    double val=0.0;
    for (int cnt=0; cnt<ln; cnt++) {
      val = this->ray[cnt];
      printf("%f, ",val);
    }
    printf("\n");
  }
  /* ********************************************************************** */
  double Compare(VectPtr other){// strictly for genalg scoring
    int ln = std::min(this->len, other->len);
    double range = 2.0;
    double val0, val1, digival0, digival1, diff, digidiff, digiscore;
    double singlescore, score = 1.0;
    for (int cnt=0;cnt<ln;cnt++){
      digival0 =  std::copysign(1.0, this->ray[cnt]);
      digival1 =  std::copysign(1.0, other->ray[cnt]);
      digidiff=std::fabs(digival0-digival1);
      digiscore+=(range-digidiff)/range;
      val0 =  this->ray[cnt];
      val1 =  other->ray[cnt];
      diff=std::fabs(val0-val1);
      singlescore=(range-diff)/range;
      score*=singlescore;
    }
    return score;
  }
};

/* ********************************************************************** */
class Matrix;// forward
typedef Matrix *MatrixPtr;
class Matrix {
public:
  int wdt,hgt;
  VectPtr* ray;
  VectPtr midvec;// persistent fire state
  /* ********************************************************************** */
  Matrix(int wdt0, int hgt0) {
    this->wdt = wdt0; this->hgt = hgt0;
    this->ray = allocsafe(VectPtr, wdt*hgt);
    for (int cnt=0; cnt<this->hgt; cnt++) {
      this->ray[cnt] = new Vect(this->wdt);
    }
    midvec = new Vect(this->wdt);
  }
  /* ********************************************************************** */
  ~Matrix() {
    for (int cnt=0; cnt<this->hgt; cnt++) {
      delete this->ray[cnt];
    }
    freesafe(this->ray);
    delete this->midvec;
  }
  /* ********************************************************************** */
  MatrixPtr Clone_Me() {
    MatrixPtr child = new Matrix(this->wdt, this->hgt);
    child->Copy_From(this);
    return child;
  }
  /* ********************************************************************** */
  void Copy_From(MatrixPtr source) {
    VectPtr src;
    for (int cnt=0; cnt<this->hgt; cnt++) {
      src=source->ray[cnt];
      this->ray[cnt]->Copy_From(src);
    }
  }
  /* ********************************************************************** */
  void Init_Identity() {// create identity matrix
    VectPtr vec;
    for (int cnt=0; cnt<this->hgt; cnt++) {
      vec=this->ray[cnt];
      vec->Fill(0.0);
      vec->ray[cnt]=1.0;
    }
  }
  /* ********************************************************************** */
  void MultFire(Vect* invec, Vect* outvec) {
    VectPtr vec;
    double Fire;
    for (int cnt=0; cnt<this->hgt; cnt++) {
      vec=this->ray[cnt];
      Fire = vec->MultFire(invec);
      outvec->ray[cnt]=Fire;
    }
  }
  /* ********************************************************************** */
  void Iterate(Vect* invec, int NumCycles, Vect* outvec) {// recurrent firing
    Vect tempvec(this->hgt);
    tempvec.Copy_From(invec);// safe, more flexible
    for (int cnt=0; cnt<NumCycles; cnt++) {// pass through 3 times total
      this->MultFire(&tempvec, outvec);
      tempvec.Copy_From(outvec);
    }
  }
  /* ********************************************************************** */
  void Iterate_Persistent(Vect* invec, Vect* outvec) {// recurrent firing
    this->midvec->Copy_From(invec);// invec is shorter than midvec, so some recurrent info carries over from last call
    for (int cnt=0; cnt<3; cnt++) {// pass through 3 times total
      this->MultFire(this->midvec, outvec);
      this->midvec->Copy_From(outvec);
    }
  }
  /* ********************************************************************** */
  void Iterate_old(Vect* invec, Vect* outvec) {// recurrent firing
    Vect tempvec(this->hgt);
    this->MultFire(invec, outvec);// not safe
    for (int cnt=1; cnt<3; cnt++) {// pass through 3 times total
      tempvec.Copy_From(outvec);
      this->MultFire(&tempvec, outvec);
    }
  }
  /* ********************************************************************** */
  void Iterate_old2(Vect* invec, Vect* outvec) {// recurrent firing
    Vect tempvec(this->hgt);
    this->MultFire(invec, &tempvec);// safe, more efficient
    for (int cnt=1; cnt<3; cnt++) {// pass through 3 times total
      this->MultFire(&tempvec, outvec);
      tempvec.Copy_From(outvec);
    }
  }
  /* ********************************************************************** */
  void Rand_Init() {
    for (int cnt=0; cnt<this->hgt; cnt++) {
      this->ray[cnt]->Rand_Init();
    }
  }
  /* ********************************************************************** */
  void Mutate_Me(double MRate) {
    for (int cnt=0; cnt<this->hgt; cnt++) {
      this->ray[cnt]->Mutate_Me(MRate);
    }
    // also allow shuffling vectors or maybe even columns here. at what rate?
    MRate *= MRate;// reduce it by square. because why not?
    VectPtr temp;
    int swapdex;
    for (int cnt=0; cnt<this->hgt; cnt++) {
      if (frand()<MRate) {
        swapdex = std::rand()%this->hgt;
        temp = this->ray[cnt];// swap
        this->ray[cnt] = this->ray[swapdex];
        this->ray[swapdex]=temp;
      }
    }
  }
  /* ********************************************************************** */
  void Mutate_Me() {
    double MRate=0.3;
    this->Mutate_Me(MRate);
  }
  /* ********************************************************************** */
  void Print_Me() {
    for (int cnt=0; cnt<this->hgt; cnt++) {
      this->ray[cnt]->Print_Me();
    }
  }
};

#if 0
class Matrix {
public:
  int wdt,hgt;
  double* ray;
  Matrix(int wdt0, int hgt0) {
    this->wdt = wdt0; this->hgt = hgt0;
    this->ray = allocsafe(double, wdt*hgt);
  }
  Matrix():Matrix(4,4) {
  }
  ~Matrix() {
    freesafe(this->ray);
  }
};
#endif // 0

#endif // MATRIX_HPP_INCLUDED

#if 0
/*
one matrix is enough for all behaviors, if it is recurrent.

so next vectors and matrices can mutate.  then maybe cross.

each network will have X outputs that are also inputs (recurrent). maybe 2? 3?
each network will read the fire values of both upstream and downstream, and know the past through recurrence.
each network will output a value that will be added to the weight of the link it controls.
that it?
but really each creature is not altering weights, it is pushing correctors backward.
it receives a corrector, and pushes it upstream. the upstreamer must add all the corrs together.

next step:
be able to run it on 2 nodes
therefore define node, and link.
creature lives inside the link.
link needs a way to load its values into the creature,
and to get the new values out.
creature should not be blanked between fires?  carry over of prev fires allows delta/hebb stuff.
if not blanked then invec and outvec should be smaller than matrix wdt, hgt.
we would need a persistent tempvec for iterate. midvec. and the midvec must belong to the creature/link, not the matrix genome.

since we will be blanking at first, prev fires will be ignored. that may not be workable.

*/
#endif // 0
