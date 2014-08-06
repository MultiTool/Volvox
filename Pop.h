#ifndef POP_H_INCLUDED
#define POP_H_INCLUDED

//#include "Org.h"
#include "Lugar.h"
#include "Base.h"
#include "FunSurf.h"
#include "Node.h"
#include "Cluster.h"
//#include "Stack.h"

//#define popmax 1000
#define popmax 100
//#define popmax 10

/* ********************************************************************** */
class Comper;
typedef Comper *ComperPtr;
class Comper {
public:
  NodePtr teacher, student;
  double CompareAnalog() { // or score
    return teacher->FireVal * student->FireVal;
  }
  double CompareDigital() {
    double direction = teacher->FireVal * student->FireVal;
    if (direction>0){return 1.0;}
    if (direction<0){return -1.0;}
    return 0.0;
  }
};
/* ********************************************************************** */
class Pop;
typedef Pop *PopPtr;
class Pop {
public:
  uint32_t popsz;
  LugarVec forestv;
  OrgVec ScoreDexv; // for sorting
  typedef struct ScorePair { double Score[2]; };
  std::vector<ScorePair> ScoreBuf;// for recording scores even after some creatures are dead
  std::vector<ComperPtr> CompPairs;
  ClusterPtr ClayNet;// crucible
  ClusterPtr Mirror;// Entorno
  uint32_t ClusterSize = 10;
  uint32_t MaxNeuroGens = 2000;
  uint32_t DoneThresh = 32;//64; //32; //64;// 128;//16;
  double avgnumwinners = 0.0;
  /* ********************************************************************** */
  Pop() : Pop(popmax) {
  }
  /* ********************************************************************** */
  Pop(int popsize) {
    ClayNet = new Cluster(ClusterSize);
    ClayNet->Randomize_Weights();

    Mirror = new Cluster(ClusterSize);
    Mirror->Randomize_Weights();

    LugarPtr lugar;
    Org *org;
    int pcnt;
    ClayNet->Connect_Self();
    Mirror->Connect_Self();
    Attach_Mirror();

    this->popsz = popsize;
    forestv.resize(popsize);
    ScoreDexv.resize(popsize);
    ScoreBuf.resize(popsize);
    for (pcnt=0; pcnt<popsize; pcnt++) {
      lugar = new Lugar();
      org = Org::Abiogenate();
      lugar->Attach_Tenant(org);
      forestv.at(pcnt) = lugar;
      ScoreDexv.at(pcnt) = org;
    }
  }
  /* ********************************************************************** */
  ~Pop() {
    size_t siz, pcnt;
    siz = forestv.size();
    for (pcnt=0; pcnt<siz; pcnt++) {
      delete forestv.at(pcnt);
    }
    delete ClayNet;
    Clear_Compers();
  }
  /* ********************************************************************** */
  void Clear_Compers() {
    for (size_t cnt=0; cnt<CompPairs.size(); cnt++) {
      delete CompPairs.at(cnt);
    }
    CompPairs.clear();
  }
  /* ********************************************************************** */
  void Attach_Mirror() {
    size_t cnt;
    size_t start = 0, finish = 0;
    Clear_Compers();
    NodePtr NodeUs, NodeDs;
    for (cnt=start; cnt<finish; cnt++) {
      NodeUs = Mirror->NodeList.at(cnt);
      NodeDs = ClayNet->NodeList.at(cnt);
      NodeDs->ConnectIn(NodeUs);
      ComperPtr comp = new Comper();
      comp->teacher = NodeUs; comp->student = NodeDs; //{ teacher = NodeUs, student = NodeDs };
      CompPairs.push_back(comp);
    }
  }
  /* ********************************************************************** */
  void Compare_Outputs(double *MajorScore, double *MinorScore) {
    ComperPtr comp;
    double score0 = 1.0;
    double score1 = 1.0;
    size_t siz = this->CompPairs.size();
    size_t NCnt;
    for (NCnt=0; NCnt<siz; NCnt++) {
      comp = this->CompPairs.at(NCnt);
      score0 *= (1.0 + comp->CompareDigital())*0.5;
      score1 *= (1.0 + comp->CompareAnalog())*0.5;
    }
    (*MajorScore) = score0; (*MinorScore) = score1;// this is a maybe. need to think it through more.
  }
  /* ********************************************************************** */
  void Run_Test(OrgPtr FSurf) {
    uint32_t FinalFail = 0;
    uint32_t GenCnt;
    double goal;
    double MajorScore, MinorScore;
    double ScoreBefore;
    double WinCnt;    Mirror->Randomize_Weights();
    ClayNet->Randomize_Weights();
    ClayNet->Attach_FunSurf(FSurf);
    WinCnt=0.0;
    for (GenCnt=0; GenCnt<MaxNeuroGens; GenCnt++) {
      ClayNet->Fire_Gen();
      Mirror->Fire_Gen();
      Compare_Outputs(&MajorScore, &MinorScore);
      FSurf->Score[0]=MajorScore;
      FSurf->Score[1]=MinorScore;
#if false
      double fire = ClayNet->OutLayer->NodeList.at(0)->FireVal;
      // how to judge performance? we need to observe the difference between the two outputs.
      // the whole bignet needs a special list of all the talky nodes - JUST FOR JUDGEMENT. could be only a range of indexes, 0 to agun cosa
      // comparison list could be owned by population, and not by either network.
      // class comper?
      if (goal*fire>0) {
        WinCnt++;
      } else {
        FinalFail = GenCnt;
      }
      if ((GenCnt-FinalFail)>DoneThresh) {
        break;
      }
      // ClayNet->Backprop(goal);
      ClayNet->Backprop(&(Pair->goalvec));
#endif
    }
    double PrimaryScore = 0;
    if (FinalFail>=(MaxNeuroGens-DoneThresh)) {
      PrimaryScore = 0.0;
    } else {
      PrimaryScore = 1.0 - ( ((double)FinalFail)/(double)(MaxNeuroGens-DoneThresh) );// oneify
    }
    FSurf->FinalFail = FinalFail;
    FSurf->Score[0] *= PrimaryScore;//1.0 - ( ((double)FinalFail)/(double)MaxNeuroGens );// oneify
    double Remainder = MaxNeuroGens-GenCnt;// if nobody won *earlier*, then score by average goodness of output
    double temp = ( (WinCnt+Remainder)/((double)MaxNeuroGens) ) - ScoreBefore;
    temp = (temp+1.0)/2.0;
    if (temp<0.0) {temp=0.0;}
    FSurf->Score[1] *= temp;//oneify
  }
  double fred;
  /*

  scoring issue for multiple training sets
  do we just change training sets once for each generation?
  or, do we expose each generation to all of the training sets, and try to make a sum score?


  */
  /* ********************************************************************** */
  void Gen(uint32_t evogens, uint32_t gencnt) { // each generation
    uint32_t popsize = this->forestv.size();
    LugarPtr lugar;
    OrgPtr parent, child, candidate;
    uint32_t pcnt;
    LugarPtr place;
    int numwinners;
    printf("Pop.Gen(), ");
    printf("\n");
    this->ClayNet->Print_Specs();
    for (pcnt=0; pcnt<popsize; pcnt++) {
      lugar = forestv[pcnt];
      candidate = lugar->tenant;
      candidate->Clear_Score();
      candidate->Oneify_Score();
      this->Run_Test(candidate);
      // printf("candidate->Score:%lf, %lf\n", candidate->Score[0], candidate->Score[1]);
    }
    double SurvivalRate = 0.5;
    Sort();
    Record_Scores();
    OrgPtr bestbeast = ScoreDexv[0];
    OrgPtr leastbeast = ScoreDexv[this->popsz-2];
    double avgbeast = AvgBeast();
    numwinners = NumWinners();
    avgnumwinners = (avgnumwinners*0.98) + ((double)numwinners)*0.02;
    bestbeast->Print_Me(); printf("\n");
    printf("bestbeast->Score:%lf, %lf\n", bestbeast->Score[0], bestbeast->Score[1]);
    printf("avgbeast Score:%lf, numwinners:%li, avgnumwinners:%lf\n", avgbeast, numwinners, avgnumwinners);
    printf("leastbeast->Score:%lf, %lf\n", leastbeast->Score[0], leastbeast->Score[1]);
    if (Org::Baselining) {
      printf("Baselining, NO MUTATION \n");// coasting, no evo
    } else {
      Birth_And_Death(SurvivalRate);
      if (32<(evogens - gencnt)) {
        Mutate(0.8, 0.8);
        printf("Mutation \n");
      } else {
        printf("NO MUTATION \n");// coasting, no evo
      }
    }
  }
  /* ********************************************************************** */
  double AvgBeast() {
    size_t siz = ScoreDexv.size();
    double sum = 0.0;
    for (int cnt=0; cnt<siz; cnt++) {
      sum += ScoreDexv[cnt]->Score[0];
    }
    sum /= (double)siz;
    return sum;
  }
  /* ********************************************************************** */
  size_t NumWinners() {
    size_t siz = ScoreDexv.size();// only works if sorted descending already
    double OneScore;
    size_t wincnt = 0;
    for (int cnt=0; cnt<siz; cnt++) {
      OneScore = ScoreDexv[cnt]->Score[0];
      // if (ScoreDexv[cnt]->FinalFail >= (MaxNeuroGens-DoneThresh)) { break; }
      //if (ScoreDexv[cnt]->Score[0]<0.01) { break; }
      //if (ScoreDexv[cnt]->FinalFail < (MaxNeuroGens-DoneThresh)) { wincnt++; }
      //if (ScoreDexv[cnt]->Score[0]>Fudge) {
      if (0.0<OneScore && OneScore<=Fudge) {
        printf("Gotta fix how we count winners!\n");
        // throw 20;
      }
      if (OneScore>=Fudge) {
        // printf("ScoreDexv[%li]->Score[0]:%lf, wincnt:%li \n", cnt, ScoreDexv[cnt]->Score[0], wincnt);
        wincnt++;
      }
      //printf(" FinalFail:%li\n", ScoreDexv[cnt]->FinalFail);
      //wincnt++;
    }
    return wincnt;
  }
  /* ********************************************************************** */
  static bool AscendingScore(OrgPtr b0, OrgPtr b1) {
    return b0->Compare_Score(b1) > 0;
  }
  static bool DescendingScore(OrgPtr b0, OrgPtr b1) {
    return b1->Compare_Score(b0) > 0;
  }
  void Sort() {
    std::random_shuffle(ScoreDexv.begin(), ScoreDexv.end());
    std::sort (ScoreDexv.begin(), ScoreDexv.end(), DescendingScore);
  }
  /* ********************************************************************** */
  void Birth_And_Death(double SurvivalRate) {
    size_t siz = ScoreDexv.size();
    size_t NumSurvivors = siz * SurvivalRate;
    size_t topcnt, cnt;
    LugarPtr home;
    OrgPtr doomed, child;
    topcnt = 0;
    for (cnt=NumSurvivors; cnt<siz; cnt++) {
      doomed = ScoreDexv[cnt]; home = doomed->home;
      delete doomed;
      child = ScoreDexv[topcnt]->Spawn();
      home->Attach_Tenant(child); ScoreDexv[cnt] = child;
      topcnt++;
      if (topcnt>=NumSurvivors) {topcnt=0;}
    }
  }
  /* ********************************************************************** */
  void Record_Scores() {
    size_t siz = ScoreDexv.size();
    double *Score, *src;
    for (int cnt=0; cnt<siz; cnt++) {
      Score = ScoreBuf.at(cnt).Score;
      src = ScoreDexv[cnt]->Score;
      Score[0] = src[0];
      Score[1] = src[1];
    }
  }
  /* ********************************************************************** */
  void Print_Sorted_Scores() {
    double *Score;
    size_t siz = ScoreDexv.size();
    int cnt;
    for (cnt=0; cnt<siz; cnt++) {
      // ScoreDexv[cnt]->Print_Score();
      Score = ScoreBuf.at(cnt).Score;
      printf(" Score:%lf, %lf\n", Score[0], Score[1]);
    }
  }
  /* ********************************************************************** */
  void Mutate_Sorted(double Pop_MRate, double Org_MRate) {
    size_t siz = this->forestv.size();
    for (int cnt=16; cnt<siz; cnt++) {
      if (frand()<Pop_MRate) {
        OrgPtr org = this->ScoreDexv[cnt];
        org->Mutate_Me(Org_MRate);
      }
    }
  }
  /* ********************************************************************** */
  void Mutate(double Pop_MRate, double Org_MRate) {
    OrgPtr org;
    size_t LastOrg;
    //size_t siz = this->forestv.size();
    size_t siz = this->ScoreDexv.size(); LastOrg = siz-1;
    for (int cnt=0; cnt<LastOrg; cnt++) {
      if (frand()<Pop_MRate) {
        //LugarPtr lugar = this->forestv.at(cnt);
        org = this->ScoreDexv[cnt];// lugar->tenant;
        org->Mutate_Me(Org_MRate);
      }
    }
    org = this->ScoreDexv[LastOrg];// very last mutant is 100% randomized, to introduce 'new blood'
    org->Rand_Init();
  }
  /* ********************************************************************** */
  void Compile_Me() {
    size_t siz = this->forestv.size();
    for (int cnt=0; cnt<siz; cnt++) {
      LugarPtr lugar = this->forestv.at(cnt);
      OrgPtr org = lugar->tenant;
      org->Compile_Me();
    }
  }
};

#endif // POP_H_INCLUDED

