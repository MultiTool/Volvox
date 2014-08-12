#ifndef ORG_H_INCLUDED
#define ORG_H_INCLUDED

#include "Base.h"
#include "Cluster.h"
#include "FunSurf.h"
#include "OrgProto.h"

/*
Every org cluster can connect to
1 my link weight and fire
2 my link's upstream meeting post,
3 my link's downstream meeting post.

does anyone need to know if I am upstream or downstream?
if all connections are 2 way (2 links), then there are 2 orgs connecting to same post for every node nbr

original idea was more vague.  2 links per, but each one's org meets the other in the middle. each org has a 2-org listening post
so you have 2 orgs leaping between 2 links to talk to each other. how do the matching ones find each other?
simpler for each org to just talk to 2 nodes directly.
different posts for instars and outstars?
doesn't matter. every org's upstream and downstream plugs will be different standards, and can occupy different regions of a single post if need be.

but, an org must specify if its IO plug is going to ustream or dstream.

one flag for each org's node:
talk to ustream
talk to dstream
talk to link itself


*/

/* ********************************************************************** */
class Org;
typedef Org *OrgPtr;
typedef std::vector<OrgPtr> OrgVec;
class Org : public Cluster, public OrgProto {
public:
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
    NodeKit<>::NodePtr USNode, DSNode;
    USNode = My_Link->USNode;
    DSNode = My_Link->DSNode;
  }
};

#endif // ORG_H_INCLUDED
