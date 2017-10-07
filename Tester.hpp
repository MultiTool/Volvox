#ifndef TESTER_HPP_INCLUDED
#define TESTER_HPP_INCLUDED

#include "Org.hpp"
#include "Cluster.hpp"
#include "TrainingSets.hpp"

/* ********************************************************************** */
class Tester;// forward
typedef Tester *TesterPtr;
typedef std::vector<TesterPtr> TesterVec;
class Tester {// base class, does not do anything by itself
public:
  OrgPtr org;
  /* ********************************************************************** */
  Tester(){
  }
  /* ********************************************************************** */
  virtual ~Tester(){
  }
  /* ********************************************************************** */
  virtual void Reset_Input() {
  }
  /* ********************************************************************** */
  virtual void Test() {
  }
  /* ********************************************************************** */
  virtual void Test(OrgPtr candidate) {
  }
  /* ********************************************************************** */
  virtual void Print_Me() {
    printf("Tester base class should be overridden.\n");
  }
  /* ********************************************************************** */
  virtual double Dry_Run_Test() {
    return 0;
  }
};

/* ********************************************************************** */
class TesterMx;// forward
typedef TesterMx *TesterMxPtr;
typedef std::vector<TesterMxPtr> TesterMxVec;
class TesterMx : public Tester {// evolve to match an existing matrix
public:
  MatrixPtr model;// alternate crucible
  VectPtr outvec0, outvec1;
  int Iterations=3;
  const static int Num_Invecs = 20;
  VectPtr invec[Num_Invecs];
  int MxWdt, MxHgt;
  /* ********************************************************************** */
  TesterMx(int MxWdt0, int MxHgt0){
    this->MxWdt=MxHgt0; this->MxHgt=MxHgt0;
    this->model = new Matrix(MxWdt0, MxHgt0);
    this->model->Rand_Init();// mutate 100%
    for (int vcnt=0;vcnt<Num_Invecs;vcnt++){
      this->invec[vcnt] = new Vect(MxWdt);
    }
    this->Reset_Input();
    this->outvec0 = new Vect(MxHgt0);
    this->outvec1 = new Vect(MxHgt0);
  }
  /* ********************************************************************** */
  ~TesterMx(){
    delete this->outvec1;
    delete this->outvec0;
    for (int vcnt=0;vcnt<Num_Invecs;vcnt++){
      delete this->invec[vcnt];
    }
    delete this->model;
  }
  /* ********************************************************************** */
  void Reset_Input() override {
    for (int vcnt=0;vcnt<Num_Invecs;vcnt++){
      this->invec[vcnt]->Rand_Init();// mutate 100%
    }
  }
  /* ********************************************************************** */
  void Test() override {
  }
  /* ********************************************************************** */
  void Test(OrgPtr candidate) override {
    // Run the candidate and the model and compare their outputs.
    double val0, val1, diff;
    double digival0, digival1, digidiff, digiscore=0;
    double range = 2.0;
    VectPtr iv;
    double singlescore, score = 1.0;
    for (int vcnt=0;vcnt<Num_Invecs;vcnt++){
      iv = this->invec[vcnt];
      model->Iterate(iv, Iterations, outvec0);
      candidate->Iterate(iv, Iterations, outvec1);
      for (int cnt=0;cnt<this->MxHgt;cnt++){
        digival0 =  std::copysign(1.0, outvec0->ray[cnt]);
        digival1 =  std::copysign(1.0, outvec1->ray[cnt]);
        digidiff=std::fabs(digival0-digival1);
        digiscore+=(range-digidiff)/range;
        val0 =  outvec0->ray[cnt];
        val1 =  outvec1->ray[cnt];
        diff=std::fabs(val0-val1);
        singlescore=(range-diff)/range;
        score*=singlescore;
      }
    }
    candidate->Score[0]=score; candidate->Score[1]=digiscore;

    // candidate->Score[0]=digiscore; candidate->Score[1]=score;
  }
  /* ********************************************************************** */
  void Print_Me() override {
    this->model->Print_Me();
  }
};

/* ********************************************************************** */
class TesterNet;// forward
typedef TesterNet *TesterNetPtr;
typedef std::vector<TesterNetPtr> TesterNetVec;
class TesterNet : public Tester {// evolve to create a backpropagation learning rule
public:
  ClusterPtr BPNet;// crucible
  uint32_t MaxNeuroGens = 2000;
  uint32_t DoneThresh = 32;//64; //32; //64;// 128;//16;
  MatrixPtr model;// behavior to imitate
  static const int ModelWdt=4, ModelHgt=4;// size of the big framework net that holds the models
  int MxWdt, MxHgt;
  VectPtr outvec0, outvec1;
  int Iterations=3;
  const static int Num_Invecs = 20;
  VectPtr invec[Num_Invecs];
  TrainingSetList TrainingSets;
  /* ********************************************************************** */
  TesterNet(){
    this->MxWdt=ModelWdt; this->MxHgt=ModelHgt;
    this->model = new Matrix(ModelWdt, ModelHgt);
    this->model->Rand_Init();// mutate 100%
    BPNet = new Cluster();
    TrainingSets.All_Truth(2);
  }
  /* ********************************************************************** */
  ~TesterNet(){
    delete this->model;
    delete BPNet;
  }
  /* ********************************************************************** */
  void Reset_Input() override {// once per generation
  }
  /* ********************************************************************** */
  void Test() override {
  }
  /* ********************************************************************** */
  void Test(OrgPtr candidate) override {
    this->BPNet->Attach_Genome(candidate);
    VectPtr iv;
    for (int vcnt=0;vcnt<Num_Invecs;vcnt++){
      iv = this->invec[vcnt];
      model->Iterate(iv, Iterations, outvec0);
      candidate->Iterate(iv, Iterations, outvec1);
    }
    candidate->Score[0]=1;//dummy assignment
    candidate->Score[1]=1;//dummy assignment
  }
  /* ********************************************************************** */
  void Print_Me() override {
    //printf("TesterNet class not implemented yet.\n");
  }
  /* ********************************************************************** */
  double Dry_Run_Test() {
#if false
    uint32_t GenCnt;
    double goal;
    double WinCnt;
    IOPairPtr Pair;
    WinCnt=0.0;
    for (GenCnt=0; GenCnt<MaxNeuroGens; GenCnt++) {
      Pair = TSet->at(GenCnt%TSet->size());
      goal = Pair->goalvec.at(0);
      // BPNet->Load_Inputs(Pair->invec.at(0), Pair->invec.at(1), 1.0);
      BPNet->Load_Inputs(&(Pair->invec));
      BPNet->Fire_Gen();
      double fire = BPNet->OutLayer->NodeList.at(0)->FireVal;
      if (goal*fire>0) {
        WinCnt++;
      }
    }
    return WinCnt/((double)MaxNeuroGens);
#endif // false
    return 0;
  }
  /* ********************************************************************** */
  void Run_Test() {
#if false
    uint32_t FinalFail = 0;
    uint32_t GenCnt;
    double goal;
    double ScoreBefore;
    double WinCnt;
    IOPairPtr Pair;
    do {
      BPNet->Randomize_Weights();
      ScoreBefore = Dry_Run_Test(16, TSet);
    } while (ScoreBefore==1.0);
    BPNet->Attach_Genome(FSurf);
    WinCnt=0.0;
    //MaxNeuroGens/=TSet->size();
    for (GenCnt=0; GenCnt<MaxNeuroGens; GenCnt++) {
      Pair = TSet->at(GenCnt%TSet->size());

      goal = Pair->goalvec.at(0);
      BPNet->Load_Inputs(&(Pair->invec));
      BPNet->Fire_Gen();
      double fire = BPNet->OutLayer->NodeList.at(0)->FireVal;
      if (goal*fire>0) {
        WinCnt++;
      } else {
        FinalFail = GenCnt;
      }
      if ((GenCnt-FinalFail)>DoneThresh) {
        break;
      }
      // BPNet->Backprop(goal);
      BPNet->Backprop(&(Pair->goalvec));
      //}
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
    if (temp<0.0) {
      temp=0.0;
    }
    FSurf->Score[1] *= temp;//oneify
#endif // false
  }
};

#endif // TESTER_HPP_INCLUDED
