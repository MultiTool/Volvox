#ifndef TESTER_SOUNDER_HPP_INCLUDED
#define TESTER_SOUNDER_HPP_INCLUDED

#include "Tester.hpp"

// Pack outputs from all creatures of a generation into a single long vector to squeeze out latencies of hardware IO.

/* ********************************************************************** */
class Tester_Sounder;// forward
typedef Tester_Sounder *Tester_SounderPtr;
typedef std::vector<Tester_SounderPtr> Tester_SounderVec;
class Tester_Sounder : public Tester {// evolve to create a vector with max wave energy (max changes, as we define it)
public:
  size_t PopSize, OrgCnt, OrgVecLen, IOVecLen;
  std::vector<OrgPtr> OrgList;
  std::vector<double> OutVec, InVec;
  /* ********************************************************************** */
  Tester_Sounder(){
    this->OrgCnt=0;
  }
  /* ********************************************************************** */
  ~Tester_Sounder(){
  }
  /* ********************************************************************** */
  void Clear_Model() {
  }
  /* ********************************************************************** */
  void Assign_Pop_Size(size_t PopSize0, size_t OrgVecLen0) {
    this->PopSize = PopSize0;
    this->OrgVecLen = OrgVecLen0;
    this->IOVecLen = this->PopSize*this->OrgVecLen;
    this->OrgList.resize(this->PopSize);
    this->InVec.resize(this->IOVecLen);
    this->OutVec.resize(this->IOVecLen);
  }
  /* ********************************************************************** */
  void Generation_Start() override {// once per generation
      this->OrgCnt=0;
  }
  /* ********************************************************************** */
  void Generation_Finish() override {// once per generation
      {// simulate that the outvec has looped through our object and returned as invec.
        std::copy_n(this->OutVec.begin(), this->IOVecLen, this->InVec.begin());
        // to do: we have to simulate latency too, and adjust for that.
      }
      Vect buf(this->OrgVecLen);
      double Energy;
      size_t SoundDex;
      OrgPtr candidate;
      for (size_t OrgDex=0;OrgDex<this->OrgCnt;OrgDex++){
        candidate = OrgList[OrgDex];
        SoundDex = OrgDex*this->OrgVecLen;
        std::copy_n(this->InVec.begin() + SoundDex, this->OrgVecLen, buf.ray.begin());
        Energy = buf.GetWaveEnergy()/((double)buf.len-1);// len-1 because we only measure differences between numbers, always one less.
        Energy *= 0.5;
        candidate->Score[0]=Energy;// to do: find a scoring system whose max is 1.0
      }
      std::fill (this->OutVec.begin(), this->OutVec.end(), 0);
      std::fill (this->InVec.begin(), this->InVec.end(), 0);
      this->OrgCnt=0;
  }
  /* ********************************************************************** */
  void Test(OrgPtr candidate) override {
    OrgList[this->OrgCnt] = candidate;
    VectPtr vect = candidate->ray[0];
    vect->Clip_Me(1.0);// hacky. here we modify the Org's genome in a test.
    size_t SoundDex = this->OrgCnt*this->OrgVecLen;
    std::copy_n(vect->ray.begin(), this->OrgVecLen, this->OutVec.begin() + SoundDex);
//    std::copy(vect->ray, vect->ray + this->OrgVecLen, this->OutVec  );
//    std::copy(vect->ray, vect->ray + this->OrgVecLen, this->OutVec[SoundDex]  );
//    std::memcpy(this->OutVec[SoundDex], vect->ray, this->OrgVecLen);
    this->OrgCnt++;
  }
  /* ********************************************************************** */
  void Print_Me() override {
  }
  /* ********************************************************************** */
  void Print_Org(OrgPtr candidate) override {
  }
  /* ********************************************************************** */
  void Profile_Model(MatrixPtr CurrentModel) override {
  }
  /* ********************************************************************** */
  void Attach_StartingState(VectPtr StartingState0) override {
  }
  /* ********************************************************************** */
  void Attach_Model(MatrixPtr model0) override {
  }
};

#endif // TESTER_SOUNDER_HPP_INCLUDED
