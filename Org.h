#ifndef ORG_H_INCLUDED
#define ORG_H_INCLUDED

#include "Base.h"
#include "Cluster.h"
#include "FunSurf.h"
#include "OrgProto.h"

/* ********************************************************************** */
class Org;
typedef Org *OrgPtr;
typedef std::vector<OrgPtr> OrgVec;
//class Org : public FunSurfGrid{
class Org : public Cluster, public OrgProto {
public:
  //LinkProtoPtr My_Link;
  LinkPtr My_Link;
  const static int NumScores = 2;
  double Score[NumScores];
  uint32_t FinalFail;
  struct Lugar *home;// my location
  static const bool Baselining = false;
  /* ********************************************************************** */
  Org() {
  }
  /* ********************************************************************** */
  //Org(uint32_t NumDims0, uint32_t Rez0) : FunSurfGrid(NumDims0, Rez0) {
  Org(uint32_t NumDims0, uint32_t Rez0) : Cluster() {
    for (int cnt=0; cnt<NumScores; cnt++) {
      this->Score[cnt] = 0.0;
    }
    this->home = NULL;
    this->FinalFail = 999999999;
  }
  /* ********************************************************************** */
  ~Org() {
  }
  /* ********************************************************************** */
  static OrgPtr Abiogenate() {
    OrgPtr org = new Org();
    org->Rand_Init();
    if (Baselining) {// snox for testing
    }
    return org;
  }
  /* ********************************************************************** */
  void Rand_Init() {
    this->Fill_With_Nodes(10);
    this->Randomize_Weights();
  }
  /* ********************************************************************** */
  void Mutate_Me(double MRate) {
    if (Baselining) {return;}
    double MutAmp = 0.01;
    double HalfAmp = MutAmp/2.0;
    uint32_t siz = this->NodeList.size();
    uint32_t cnt;
  }
  /* ********************************************************************** */
  OrgPtr Spawn() {
    OrgPtr child;
    child = new Org();
    return child;
  }
  /* ********************************************************************** */
  void Compile_Me() {
  }
  /* ********************************************************************** */
  void Clear_Score() {
    for (int cnt=0; cnt<NumScores; cnt++) {
      this->Score[cnt]=0.0;
    }
  }
  /* ********************************************************************** */
  void Oneify_Score() { // this is for accumulating scores by multiplication: Score *= subscore
    for (int cnt=0; cnt<NumScores; cnt++) {
      this->Score[cnt]=1.0;
    }
  }
  /* ********************************************************************** */
  void Rescale_Score(double Factor) {
    for (int cnt=0; cnt<NumScores; cnt++) {
      this->Score[cnt]*=Factor;
    }
  }
  /* ********************************************************************** */
  void Print_Me() override {
    printf("Org\n");
  }
  /* ********************************************************************** */
  void Print_Score() {
    printf(" Score:%lf, %lf\n", this->Score[0], this->Score[1]);
  }
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
  void Attach_Link(LinkPtr lnk) override {
    My_Link = lnk;
  }
  /* ********************************************************************** */
  void Run_Cycle() override {
    double Fire = My_Link->FireVal;// I can read this
    double Weight = My_Link->Weight;// but only write to this
    NodePtr USNode, DSNode;
    USNode = My_Link->USNode;
    DSNode = My_Link->DSNode;
  }
};

#endif // ORG_H_INCLUDED
