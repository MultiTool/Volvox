#ifndef TESTER_HPP_INCLUDED
#define TESTER_HPP_INCLUDED

#include "Org.hpp"
#include "Cluster.hpp"
// #include "TrainingSets.hpp"

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
class TesterMxLoop;// forward
typedef TesterMxLoop *TesterMxLoopPtr;
typedef std::vector<TesterMxLoopPtr> TesterMxLoopVec;
class TesterMxLoop : public Tester {// evolve to create a mirror of a continuous behavior
public:
  const static uint32_t TestRuns = 100;
  int External_Node_Number=-1, Total_Node_Number=-1;
  MatrixPtr model;// behavior to imitate
  int MxWdt, MxHgt;
  int ModelIterations=1;
  VectPtr ModelStateSeed;
  /* ********************************************************************** */
  TesterMxLoop(int MxWdt0, int MxHgt0){
    this->MxWdt=MxHgt0; this->MxHgt=MxHgt0;
    this->model = new Matrix(MxWdt0, MxHgt0);
    External_Node_Number=MxWdt0/2; Total_Node_Number=MxWdt0-External_Node_Number;
    Scramble_Model();
    printf("Model:\n");
    this->model->Print_Me();
    printf("\n");
    ModelStateSeed = new Vect(Total_Node_Number);
    ModelStateSeed->Rand_Init();
  }
  /* ********************************************************************** */
  ~TesterMxLoop(){
    delete ModelStateSeed;
    delete this->model;
  }
  /* ********************************************************************** */
  void Scramble_Model() {// once per generation
    double Mag=0.0;
    do {
      this->model->Rand_Init();// mutate 100%
      Mag = this->model->Magnitude();
    } while (Mag<2.0);
  }
  /* ********************************************************************** */
  void Reset_Input() override {// once per generation
    this->ModelStateSeed->Rand_Init();
  }
  /* ********************************************************************** */
  void Test(OrgPtr candidate) override {
    Vect ModelState(Total_Node_Number);
    Vect OrgState(Total_Node_Number);
    ModelState.Copy_From(ModelStateSeed);
    double onescore, score, digiscore, sumdigiscore;
    int OneBitDex = External_Node_Number-1;
    double PerfectDigi = External_Node_Number*TestRuns;// maximum possible digital score
    static const bool addbit = true;
    int vcnt=0;
    if (false){
      while (vcnt<10){// running start
        if (addbit){
          ModelState.ray[OneBitDex]=1.0;
        }
        OrgState.Copy_From(&ModelState, External_Node_Number);// duplicate inputs so model and network have the same inputs
        model->Iterate(&ModelState, ModelIterations, &ModelState);
        candidate->Iterate(&OrgState, ModelIterations, &OrgState);
        vcnt++;
      }
    }

    // Scoring loop
    score=1.0;
    sumdigiscore=0;
    while (vcnt<TestRuns){
      if (addbit){
        ModelState.ray[OneBitDex]=1.0;
      }
      OrgState.Copy_From(&ModelState, External_Node_Number);// duplicate inputs so model and network have the same inputs
      model->Iterate(&ModelState, ModelIterations, &ModelState);
      candidate->Iterate(&OrgState, ModelIterations, &OrgState);
      // here we want to compare the outputs and score the Org. compare outvec with the external parts of ModelState
      onescore = OrgState.Score_Similarity(&ModelState, External_Node_Number, digiscore);
      if (onescore>1.0){
        printf("Tester error:%f",onescore);
      }
      score *= onescore;
      if (score>1.0){
        printf("Tester error:%f",score);
      }
      sumdigiscore+=digiscore;
      vcnt++;
    }
    if (true){// score mainly by product of all closenesses.
      candidate->Score[0]=score;
      //candidate->Score[1]=sumdigiscore;
      candidate->Score[1]=sumdigiscore/PerfectDigi;
    }else{// score mainly by digital difference
      candidate->Score[0]=sumdigiscore/PerfectDigi;
      candidate->Score[1]=score;
    }
  }
  /* ********************************************************************** */
  void Print_Me() override {
    //printf("TesterMxLoop class not implemented yet.\n");
  }
  /* ********************************************************************** */
  double Dry_Run_Test() {
    return 0;
  }
  /* ********************************************************************** */
  void Run_Test() {
  }
};

/* ********************************************************************** */
class TesterNet;// forward
typedef TesterNet *TesterNetPtr;
typedef std::vector<TesterNetPtr> TesterNetVec;
class TesterNet : public Tester {// evolve to create a backpropagation learning rule
public:
  ClusterPtr BPNet;// crucible
  //const static uint32_t MaxNeuroGens = 1000;//100;//2000;
  const static uint32_t MaxNeuroGens = 0;//100;//2000;
  const static uint32_t TestRuns = 100;// 10
  uint32_t DoneThresh = 32;//64; //32; //64;// 128;//16;
  //static const int External_Node_Number=2, Total_Node_Number=External_Node_Number+3;
  static const int External_Node_Number=3, Total_Node_Number=External_Node_Number*2;
  MatrixPtr model;// behavior to imitate
  static const int ModelWdt=Total_Node_Number, ModelHgt=Total_Node_Number;// size of the big framework net that holds the models
  int MxWdt, MxHgt;
  int ModelIterations=1;
  const static int Num_Invecs = 20;
  VectPtr ModelStateSeed;//Total_Node_Number);
  /* ********************************************************************** */
  TesterNet(){
    this->MxWdt=ModelWdt; this->MxHgt=ModelHgt;
    this->model = new Matrix(ModelWdt, ModelHgt);
    Scramble_Model();
    printf("Model:\n");
    this->model->Print_Me();
    printf("\n");
    if (false){
      BPNet = new Cluster(Total_Node_Number);
      //BPNet->Connect_Other_Cluster(BPNet);// all to all self. does not work well.
      BPNet->Self_Connect_Ring();
    }else{
      BPNet = new Cluster();
      BPNet->Create_Hypercube(3);
    }
    ModelStateSeed = new Vect(Total_Node_Number);
    ModelStateSeed->Rand_Init();
  }
  /* ********************************************************************** */
  ~TesterNet(){
    delete ModelStateSeed;
    delete this->model;
    delete BPNet;
  }
  /* ********************************************************************** */
  void Scramble_Model() {// once per generation
    double Mag=0.0;
    do {
      this->model->Rand_Init();// mutate 100%
      Mag = this->model->Magnitude();
      printf("Mag:%f\n", Mag);
    } while (Mag<1.0);
  }
  /* ********************************************************************** */
  void Reset_Input() override {// once per generation
    Scramble_Model();
    this->ModelStateSeed->Rand_Init();
  }
  /* ********************************************************************** */
  void Test(OrgPtr candidate) override {
    this->BPNet->Attach_Genome(candidate);
    this->BPNet->Clear_State();
    Vect ModelState(Total_Node_Number);
    Vect Xfer(External_Node_Number);//, outvec(External_Node_Number);
    ModelState.Copy_From(ModelStateSeed);
    double onescore, score, digiscore, sumdigiscore;
    int OneBitDex = External_Node_Number-1;
    double PerfectDigi = External_Node_Number*TestRuns;// maximum possible digital score
    // Learning loop
    for (int vcnt=0;vcnt<MaxNeuroGens;vcnt++){
      //ModelState.ray[OneBitDex]=1.0;
      Xfer.Copy_From(&ModelState, External_Node_Number);// duplicate inputs so model and network have the same inputs
      if (false){
        printf("ModelState1:\n");
        ModelState.Print_Me();
      }
      model->Iterate(&ModelState, ModelIterations, &ModelState);
      if (false){
        printf("ModelState2:\n");
        ModelState.Print_Me();
      }
      this->BPNet->Load_Inputs(&Xfer);
      this->BPNet->Fire_Gen();
    }
    // Scoring loop
    score=1.0;
    sumdigiscore=0;
    //ModelState.Rand_Init();
    for (int vcnt=0;vcnt<TestRuns;vcnt++){
      //ModelState.ray[OneBitDex]=1.0;
      Xfer.Copy_From(&ModelState, External_Node_Number);// duplicate inputs so model and network have the same inputs

      model->Iterate(&ModelState, ModelIterations, &ModelState);
      if (false){
        printf("ModelState3:\n");
        ModelState.Print_Me();
      }
      this->BPNet->Load_Inputs(&Xfer);
      this->BPNet->Fire_Gen();
      this->BPNet->Get_Outputs(&Xfer);
      // here we want to compare the outputs and score the Org. compare outvec with the external parts of ModelState
      onescore = Xfer.Score_Similarity(&ModelState, External_Node_Number, digiscore);
      if (onescore>1.0){
        printf("Tester error:%f",onescore);
      }
      score *= onescore;
      if (score>1.0){
        printf("Tester error:%f",score);
      }
      sumdigiscore+=digiscore;
      //printf("ModelState.Magnitude:%f, Xfer.Magnitude:%f\n", ModelState.Magnitude(), Xfer.Magnitude());
      if (false){
      //if (vcnt>2){
        printf("comparison\n");
        Xfer.Print_Me();
        ModelState.Print_Me();
      }
    }
    //printf("score:%f\n", score);
    candidate->Score[0]=score;
    //candidate->Score[1]=sumdigiscore;
    candidate->Score[1]=sumdigiscore/PerfectDigi;
/*
ok test is:
model iterates once
bpnet iterates once, in which each org iterates 3 times
a subset of bpnet output and model output are compared
model subset output then overwrites bpnet output, becomes input
*/
  }
  /* ********************************************************************** */
  void Print_Me() override {
    //printf("TesterNet class not implemented yet.\n");
  }
  /* ********************************************************************** */
  double Dry_Run_Test() {
    return 0;
  }
  /* ********************************************************************** */
  void Run_Test() {
  }
};

#endif // TESTER_HPP_INCLUDED
