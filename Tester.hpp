#ifndef TESTER_HPP_INCLUDED
#define TESTER_HPP_INCLUDED
#include <thread> // std::thread
#include <atomic> // std::atomic, std::atomic_flag, ATOMIC_FLAG_INIT

#include "Org.hpp"
// #include "Model.hpp"
#include "Cluster.hpp"
// #include "TrainingSets.hpp"

using namespace std;

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
  virtual void Generation_Start() {
  }
  /* ********************************************************************** */
  virtual void Generation_Finish() {
  }
  /* ********************************************************************** */
  virtual void Test(OrgPtr candidate) {
  }
  /* ********************************************************************** */
  virtual void Print_Me() {
    printf("Tester base class should be overridden.\n");
  }
  /* ********************************************************************** */
  virtual void Print_Org(OrgPtr candidate) {
  }
  // These are only relevant to testers that have mx models.  Should probably be split into a subclass.
  /* ********************************************************************** */
  virtual void Profile_Model(MatrixPtr CurrentModel) {
  }
  /* ********************************************************************** */
  virtual void Attach_StartingState(VectPtr StartingState0) {
  }
  /* ********************************************************************** */
  virtual void Attach_Model(MatrixPtr CurrentModel) {
  }
};

/* ********************************************************************** */
class Tester_Mx;// forward
typedef Tester_Mx *Tester_MxPtr;
typedef std::vector<Tester_MxPtr> Tester_MxVec;
class Tester_Mx : public Tester {// evolve to match an existing matrix
public:
  MatrixPtr model;// alternate crucible
  VectPtr outvec0, outvec1;
  int Iterations=3;
  const static int Num_Invecs = 20;
  VectPtr invec[Num_Invecs];
  int MxWdt, MxHgt;
  /* ********************************************************************** */
  Tester_Mx(int MxWdt0, int MxHgt0){
    this->MxWdt=MxHgt0; this->MxHgt=MxHgt0;
    this->model = new Matrix(MxWdt0, MxHgt0);
    this->model->Rand_Init();// mutate 100%
    for (int vcnt=0;vcnt<Num_Invecs;vcnt++){
      this->invec[vcnt] = new Vect(MxWdt);
    }
    this->Scramble_Invecs();
    this->outvec0 = new Vect(MxHgt0);
    this->outvec1 = new Vect(MxHgt0);
  }
  /* ********************************************************************** */
  ~Tester_Mx(){
    delete this->outvec1;
    delete this->outvec0;
    for (int vcnt=0;vcnt<Num_Invecs;vcnt++){
      delete this->invec[vcnt];
    }
    delete this->model;
  }
  /* ********************************************************************** */
  void Scramble_Invecs() {
    for (int vcnt=0;vcnt<Num_Invecs;vcnt++){
      this->invec[vcnt]->Rand_Init();// mutate 100%
    }
  }
  /* ********************************************************************** */
  void Generation_Start() override {
      //Scramble_Invecs();
  }
  /* ********************************************************************** */
  void Generation_Finish() override {
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
  void Attach_Model(MatrixPtr CurrentModel) override {
  }
};

/* ********************************************************************** */
class Tester_Mx_Loop;// forward
typedef Tester_Mx_Loop *Tester_Mx_LoopPtr;
typedef std::vector<Tester_Mx_LoopPtr> Tester_Mx_LoopVec;
class Tester_Mx_Loop : public Tester {// evolve to create a mirror of a continuous behavior
public:
  const static uint32_t RunningStart = 50;//0;//100;//2000;
  const static uint32_t TestRuns = 100;
  int External_Node_Number=-1, Total_Node_Number=-1;// deliberately crashing values
  MatrixPtr model, model_internal=nullptr;// behavior to imitate
  int MxWdt, MxHgt;
  int ModelIterations=1;
  VectPtr StartingState;
  double PerfectDigi;// maximum possible digital score
  /* ********************************************************************** */
  Tester_Mx_Loop(int MxWdt0, int MxHgt0){
    this->MxWdt=MxHgt0; this->MxHgt=MxHgt0;
    this->model = this->model_internal = new Matrix(MxWdt0, MxHgt0);
    External_Node_Number=MxWdt0/2; Total_Node_Number=MxWdt0;
    PerfectDigi = External_Node_Number*TestRuns;// maximum possible digital score
    Scramble_Model();
//    printf("Model:\n"); this->model->Print_Me();
    printf("\n");
    StartingState = new Vect(Total_Node_Number);
    StartingState->Rand_Init();
    //Profile_Model(this->model);
  }
  /* ********************************************************************** */
  ~Tester_Mx_Loop(){
    delete StartingState;
    this->Clear_Model();
  }
  /* ********************************************************************** */
  void Clear_Model() {
    if (this->model_internal!=nullptr){
       delete this->model_internal; this->model_internal = nullptr;
    }
    this->model = nullptr;
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
  void Scramble_StartingState() {
    this->StartingState->Rand_Init();
  }
  /* ********************************************************************** */
  void Generation_Start() override {// once per generation
  }
  /* ********************************************************************** */
  void Test(OrgPtr candidate) override {
    Vect ModelState(Total_Node_Number);
    Vect OrgState(candidate->wdt);
    ModelState.Copy_From(StartingState);
    double onescore, score, digiscore, sumdigiscore, DigiProduct, MultiDigiProduct;
    int OneBitDex = External_Node_Number-1;
    static const bool addbit = false;
    if (true){
      for (int vcnt=0;vcnt<RunningStart;vcnt++){// running start
        if (addbit){
          ModelState.ray[OneBitDex]=1.0;
        }
        OrgState.Copy_From(&ModelState, External_Node_Number);// duplicate inputs so model and network have the same inputs
        model->Iterate(&ModelState, ModelIterations, &ModelState);
        candidate->Iterate(&OrgState, ModelIterations, &OrgState);
      }
    }

    // Scoring loop
    score=1.0;
    sumdigiscore=0; MultiDigiProduct=1.0;
    for (int vcnt=0;vcnt<TestRuns;vcnt++){// Scoring loop
      if (addbit){
        ModelState.ray[OneBitDex]=1.0;
      }
      OrgState.Copy_From(&ModelState, External_Node_Number);// duplicate inputs so model and network have the same inputs
      model->Iterate(&ModelState, ModelIterations, &ModelState);
      candidate->Iterate(&OrgState, ModelIterations, &OrgState);
      // here we want to compare the outputs and score the Org. compare outvec with the external parts of ModelState
      onescore = OrgState.Score_Similarity(&ModelState, External_Node_Number, digiscore, DigiProduct);
      score *= onescore;
      sumdigiscore+=digiscore;
      MultiDigiProduct *= DigiProduct;
    }
    if (false){
      if (true){// score mainly by product of all closenesses.
        candidate->Score[0]=score;
        //candidate->Score[1]=sumdigiscore;
        candidate->Score[1]=sumdigiscore/PerfectDigi;
      }else{// score mainly by digital difference
        candidate->Score[0]=sumdigiscore/PerfectDigi;
        candidate->Score[1]=score;
      }
    }
    double ScoreRoot = std::pow(score, 1.0/(double)PerfectDigi);
    double DigiScoreRoot = std::pow(MultiDigiProduct, 1.0/(double)PerfectDigi);
    if (false){
      candidate->Score[0]=ScoreRoot;// analog score is primary
      candidate->Score[1]=DigiScoreRoot;// digital score
      candidate->Score[2]=sumdigiscore/PerfectDigi;
    }else{
      candidate->Score[0]=DigiScoreRoot;// digital score is primary
      candidate->Score[1]=ScoreRoot;
      candidate->Score[2]=sumdigiscore/PerfectDigi;
    }

  }
  /* ********************************************************************** */
  void Print_Me() override {
    //printf("Tester_Mx_Loop class not implemented yet.\n");
  }
  /* ********************************************************************** */
  void Print_Org(OrgPtr candidate) override {
  }
  /* ********************************************************************** */
  void Profile_Model(MatrixPtr CurrentModel) override {
    int Num_Bins = 16;
    int Bins[Num_Bins] = {};
    Vect ModelState(Total_Node_Number);
    ModelState.Copy_From(StartingState);
    ModelState.Print_Me();
    double Range = ModelState.MaxLen();
    int BinDex;
    printf("MxLoop StartingState:\n");
    ModelState.Print_Me();
    printf("RunningStart:\n");
    for (int vcnt=0;vcnt<RunningStart;vcnt++){
      CurrentModel->Iterate(&ModelState, ModelIterations, &ModelState);
    }
    printf("TestRuns:\n");
    double Percent_Negative=0.0;
    for (int vcnt=0;vcnt<TestRuns;vcnt++){
      CurrentModel->Iterate(&ModelState, ModelIterations, &ModelState);
      ModelState.Print_Me();
      Percent_Negative+=ModelState.Percent_Negative();
      BinDex = (ModelState.Magnitude()*Num_Bins)/Range;
      Bins[BinDex]++;
    }
    Percent_Negative/=(double)TestRuns;
    printf("Percent_Negative:%f\n", Percent_Negative);
    for (int bcnt=0;bcnt<Num_Bins;bcnt++){
      printf("%i, ", Bins[bcnt]);
    }
    printf("\n");
  }
  /* ********************************************************************** */
  void Attach_StartingState(VectPtr StartingState0) override {
    this->StartingState->Define_From(StartingState0);
  }
  /* ********************************************************************** */
  void Attach_Model(MatrixPtr model0) override {
    this->Clear_Model();
    this->model = model0;
    this->MxWdt=this->model->wdt; this->MxHgt=this->model->hgt;
    Total_Node_Number=this->MxWdt; External_Node_Number=Total_Node_Number/2;
  }
};

/* ********************************************************************** */
class Tester_Mx_Wobble;// forward
typedef Tester_Mx_Wobble *Tester_Mx_WobblePtr;
typedef std::vector<Tester_Mx_WobblePtr> Tester_Mx_WobbleVec;
class Tester_Mx_Wobble : public Tester {// evolve to create models with greater variance in amplitude, to be more interesting
public:
  const static uint32_t RunningStart = 50;//0;//100;//2000;
  const static uint32_t TestRuns = 100;// 10
  int Num_Bins = 16, Max_Bin = Num_Bins-1;
  int ModelIterations=1;
  int MxWdt, MxHgt;
  VectPtr StartingState;
  /* ********************************************************************** */
  Tester_Mx_Wobble(int MxWdt0, int MxHgt0){
    this->MxWdt=MxHgt0; this->MxHgt=MxHgt0;
    StartingState = new Vect(MxWdt0);
    StartingState->Rand_Init();
  }
  /* ********************************************************************** */
  ~Tester_Mx_Wobble(){
    delete StartingState;
  }
  /* ********************************************************************** */
  void Generation_Start() override {// once per generation
  }
  /* ********************************************************************** */
  void Test(OrgPtr candidate) override {
    int Bins[Num_Bins] = {};
    Vect ModelState(this->MxWdt);
    ModelState.Copy_From(StartingState);
    double Range = ModelState.MaxLen();
    double Mag;
    int BinDex;
    for (int vcnt=0;vcnt<RunningStart;vcnt++){
      candidate->Iterate(&ModelState, ModelIterations, &ModelState);
    }
    double Percent_Negative=0.0;
    for (int vcnt=0;vcnt<TestRuns;vcnt++){
      candidate->Iterate(&ModelState, ModelIterations, &ModelState);
      Percent_Negative+=ModelState.Percent_Negative();
      Mag = ModelState.Magnitude();
      BinDex = (Mag*(double)Max_Bin)/Range;
      Bins[BinDex]++;
    }
    Percent_Negative/=(double)TestRuns;
    double OneSide;
    double score = 1.0;
    for (int bcnt=0;bcnt<Num_Bins;bcnt++){
       if (true){
         score *= (0.001/(double)Num_Bins) + ((((double)Bins[bcnt])/(double)TestRuns)*0.999);
       }else{
         OneSide = (((double)Bins[bcnt])/(double)TestRuns);
         score += OneSide*OneSide;// sum of squares for alternate pythagorean approach
       }
    }
    // score = std::sqrt(score); // pythagorean.  so subtract this score from max hypotenuse?  max hypot is sqrt(Num_Bins) if max for 1 bin is 1.0.
    // ((1/16) ^ 16) ^ (1/16) *16 = 1
    // 1/Num_Bins to the power of Num_Bins
    double PerfectScore = std::pow(1.0/(double)Num_Bins, (double)Num_Bins);// 1/16 to the 16th is in fact a perfect score
    //double ScoreRoot = std::pow(score, (double)PerfectScore);
    //double PerfectScore = TestRuns;// this is dubious.  need real math, not voodoo.
    //double ScoreRoot = std::pow(score, 1.0/(double)PerfectScore);
    double ScoreRoot = std::pow(score, 1.0/(double)Num_Bins) * ((double)Num_Bins);
    candidate->Score[0]=ScoreRoot;
    candidate->Score[1]=(1.0-Percent_Negative)*Percent_Negative;// favor equal ratio of positive to negative
  }
  /* ********************************************************************** */
  void Print_Me() override {
    //printf("Tester_Mx_Loop class not implemented yet.\n");
  }
  /* ********************************************************************** */
  void Print_Org(OrgPtr candidate) override {
    printf("Top Org:\n");
    candidate->Print_Me();
    printf("\n");
    int Bins[Num_Bins] = {};
    Vect ModelState(this->MxWdt);
    ModelState.Copy_From(StartingState);
    printf("MxWobble StartingState:\n");
    ModelState.Print_Me();
    double Range = ModelState.MaxLen();
    double Mag;
    int BinDex;
    printf("RunningStart:\n");
    for (int vcnt=0;vcnt<RunningStart;vcnt++){
      candidate->Iterate(&ModelState, ModelIterations, &ModelState);
    }
    printf("TestRuns:\n");
    double Percent_Negative=0.0;
    for (int vcnt=0;vcnt<TestRuns;vcnt++){
      candidate->Iterate(&ModelState, ModelIterations, &ModelState);
      ModelState.Print_Me();
      Percent_Negative+=ModelState.Percent_Negative();
      Mag = ModelState.Magnitude();
      BinDex = (Mag*(double)Max_Bin)/Range;
      Bins[BinDex]++;
    }
    Percent_Negative/=(double)TestRuns;

    printf("Percent_Negative:%f\n", Percent_Negative);
    for (int bcnt=0;bcnt<Num_Bins;bcnt++){
      printf("%i, ", Bins[bcnt]);
    }
    printf("\n");
    printf("Score0:%24.17g, Score1:%24.17g\n", candidate->Score[0], candidate->Score[1]);
  }
  /* ********************************************************************** */
  void Attach_StartingState(VectPtr StartingState0) override {
    this->StartingState->Define_From(StartingState0);
  }
};

/* ********************************************************************** */
class Tester_Net;// forward
typedef Tester_Net *Tester_NetPtr;
typedef std::vector<Tester_NetPtr> Tester_NetVec;
class Tester_Net : public Tester {// evolve to create a backpropagation learning rule
public:
  ClusterPtr MacroNet;// crucible
  //const static uint32_t RunningStart = 1000;//100;//2000;
  const static uint32_t RunningStart = 50;//0;//100;//2000;
  const static uint32_t TestRuns = 100;// 10
  static const int External_Node_Number=3, Total_Node_Number=External_Node_Number*2;
  static const int ModelWdt=Total_Node_Number, ModelHgt=Total_Node_Number;// size of the big framework net that holds the models
  static const int Num_Models=1;
  double PerfectDigi = Num_Models*TestRuns*External_Node_Number;// maximum possible digital score
  static const int HCubeDims = 3;//4;
  std::vector<MatrixPtr> ModelVec;// behavior to imitate
  int ModelIterations=1, NetworkIterations=5;
  const static int Num_Invecs = 20;
  VectPtr StartingState;//Total_Node_Number);
  /* ********************************************************************** */
  Tester_Net(){
    printf("PerfectDigi:%f, HCubeDims:%i\n", PerfectDigi, HCubeDims);
    StartingState = new Vect(Total_Node_Number);
    this->Scramble_ModelStateSeed();
    Init_Models();
    Print_Models();
    printf("\n");
    if (false){
      //MacroNet = new Cluster(Total_Node_Number);
      MacroNet = new Cluster(0x1<<HCubeDims);
      //MacroNet->Connect_Other_Cluster(MacroNet);// all to all self. does not work well.
      //MacroNet->Self_Connect_Ring();
      MacroNet->Intraconnect_All_No_Selfie();
      printf("Intraconnect_All_No_Selfie, ");
    }else{
      MacroNet = new Cluster();
      MacroNet->Create_Hypercube(HCubeDims);
      MacroNet->Selfie();
      printf("Create_Hypercube, ");
    }
    printf("NetworkIterations:%i", NetworkIterations);
    printf("\n");
  }
  /* ********************************************************************** */
  ~Tester_Net(){
    delete StartingState;
    Delete_Models();
    delete MacroNet;
  }
  /* ********************************************************************** */
  void Scramble_Test() {
    Scramble_Models();
    this->Scramble_ModelStateSeed();
  }
  /* ********************************************************************** */
  void Generation_Start() override {// once per generation
    if (false){
      Scramble_Test();
    }
  }
  /* ********************************************************************** */
  void Test(OrgPtr candidate) override {
    this->MacroNet->Attach_Genome(candidate);
    Vect ModelState(Total_Node_Number);
    Vect Xfer(External_Node_Number);
    double onescore, score, digiscore, sumdigiscore, DigiProduct, MultiDigiProduct;
    double ModelStateMag;
    int OneBitDex = External_Node_Number-1;
    MatrixPtr CurrentModel;
    score=1.0;
    sumdigiscore=0; MultiDigiProduct=1.0;
    ModelStateMag=1.0;
    for (int mcnt=0;mcnt<Num_Models;mcnt++){
      ModelState.Copy_From(StartingState);
      this->MacroNet->Clear_State();
      CurrentModel = this->ModelVec.at(mcnt);
      for (int vcnt=0;vcnt<RunningStart;vcnt++){// Learning/adapting loop
        //ModelState.ray[OneBitDex]=1.0;
        Xfer.Copy_From(&ModelState, External_Node_Number);// duplicate inputs so model and network have the same inputs
        CurrentModel->Iterate(&ModelState, ModelIterations, &ModelState);
        this->MacroNet->Load_Inputs(&Xfer);
        for (int ncnt=0;ncnt<NetworkIterations;ncnt++){
          this->MacroNet->Fire_Gen();
        }
      }
      //ModelState.Rand_Init();
      for (int vcnt=0;vcnt<TestRuns;vcnt++){// Scoring loop
        //ModelState.ray[OneBitDex]=1.0;
        Xfer.Copy_From(&ModelState, External_Node_Number);// duplicate inputs so model and network have the same inputs
        CurrentModel->Iterate(&ModelState, ModelIterations, &ModelState);
        //ModelState.Print_Me();
        this->MacroNet->Load_Inputs(&Xfer);
        for (int ncnt=0;ncnt<NetworkIterations;ncnt++){
          this->MacroNet->Fire_Gen();
        }
        // Here we compare the outputs and score the Org. Compare outvec with the external parts of ModelState.
        this->MacroNet->Get_Outputs(&Xfer);
        onescore = Xfer.Score_Similarity(&ModelState, External_Node_Number, digiscore, DigiProduct);
        score *= onescore;
        sumdigiscore+=digiscore;
        MultiDigiProduct *= DigiProduct;
        ModelStateMag = ModelState.Magnitude();
        //printf("ModelStateMag:%24.17g\n", ModelStateMag);
        //printf("ModelStateMag:%f\n", ModelState.Magnitude());
      }
    }// loop for each model
    double ScoreRoot = std::pow(score, 1.0/(double)PerfectDigi);
    double DigiScoreRoot = std::pow(MultiDigiProduct, 1.0/(double)PerfectDigi);
    if (false){
      candidate->Score[0]=ScoreRoot;// analog score is primary
      candidate->Score[1]=sumdigiscore/PerfectDigi;
    }else{
      candidate->Score[0]=DigiScoreRoot;// digital score is primary
      candidate->Score[1]=ScoreRoot;
      candidate->Score[2]=sumdigiscore/PerfectDigi;
    }
    candidate->ModelStateMag = ModelState.Magnitude();
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
    //printf("Tester_Net class not implemented yet.\n");
  }
  /* ********************************************************************** */
  void Print_Org(OrgPtr candidate) override {
  }
  /* ********************************************************************** */
  void Profile_Model(MatrixPtr CurrentModel) override {
    int Num_Bins = 16;
    int Bins[Num_Bins] = {};
    Vect ModelState(Total_Node_Number);
    ModelState.Copy_From(StartingState);
    ModelState.Print_Me();
    double Range = ModelState.MaxLen();
    int BinDex;
    printf("RunningStart:\n");
    for (int vcnt=0;vcnt<RunningStart;vcnt++){
      CurrentModel->Iterate(&ModelState, ModelIterations, &ModelState);
      // ModelState.Print_Me();
    }
    printf("TestRuns:\n");
    double Percent_Negative=0.0;
    for (int vcnt=0;vcnt<TestRuns;vcnt++){
      CurrentModel->Iterate(&ModelState, ModelIterations, &ModelState);
      ModelState.Print_Me();
      Percent_Negative+=ModelState.Percent_Negative();
      BinDex = (ModelState.Magnitude()*Num_Bins)/Range;
      Bins[BinDex]++;
    }
    Percent_Negative/=(double)TestRuns;
    printf("Percent_Negative:%f\n", Percent_Negative);
    for (int bcnt=0;bcnt<Num_Bins;bcnt++){
      printf("%i, ", Bins[bcnt]);
    }
    printf("\n");
  }
  /* ********************************************************************** */
  void Attach_StartingState(VectPtr StartingState0) override {
    this->StartingState->Define_From(StartingState0);
  }
  /* ********************************************************************** */
  void Attach_Model(MatrixPtr CurrentModel) override {
  }
  /* ********************************************************************** */
  void Init_Models() {// once per generation
    MatrixPtr mod;
    for (int mcnt=0;mcnt<Num_Models;mcnt++){
      mod = new Matrix(ModelWdt, ModelHgt);
      Scramble_Model(mod);
      ModelVec.push_back(mod);
    }
  }
  /* ********************************************************************** */
  void Delete_Models() {
    for (int mcnt=0;mcnt<Num_Models;mcnt++){
      delete ModelVec.at(mcnt);
    }
  }
  /* ********************************************************************** */
  void Print_Models() {
    MatrixPtr mod;
    for (int mcnt=0;mcnt<Num_Models;mcnt++){
      printf("Model %i:\n", mcnt);
      mod = ModelVec.at(mcnt);
      mod->Print_Me();
      printf("Profile:\n");
      Profile_Model(mod);
    }
  }
  /* ********************************************************************** */
  void Scramble_Models() {// once per generation
    for (int mcnt=0;mcnt<Num_Models;mcnt++){
      Scramble_Model(ModelVec.at(mcnt));
    }
  }
  /* ********************************************************************** */
  void Scramble_Model(MatrixPtr mod) {// once per generation
    double Mag=0.0;
    do {
      mod->Rand_Init();// mutate 100%
      Mag = mod->Magnitude();
      printf("Mag:%f\n", Mag);
    } while (Mag<1.0);
  }
  /* ********************************************************************** */
  void Scramble_ModelStateSeed() {
    double Mag=0.0;
    do {
      StartingState->Rand_Init();// mutate 100%
      Mag = StartingState->Magnitude();
      printf("Seed Mag:%f\n", Mag);
    } while (Mag<1.0);
  }
};

/* ********************************************************************** */
class Tester_Vect;// forward
typedef Tester_Vect *Tester_VectPtr;
typedef std::vector<Tester_VectPtr> Tester_VectVec;
class Tester_Vect : public Tester {// evolve to create a vector with max wave energy (max changes, as we define it)
public:
  /* ********************************************************************** */
  Tester_Vect(){
  }
  /* ********************************************************************** */
  ~Tester_Vect(){
  }
  /* ********************************************************************** */
  void Generation_Start() override {// once per generation
  }
  /* ********************************************************************** */
  void Test(OrgPtr candidate) override {
    VectPtr vect = candidate->ray[0];
    vect->Clip_Me(1.0);// hacky. here we modify the Org's genome in a test.
    double Range = vect->MaxLen();
    double Energy;
    Energy = vect->GetWaveEnergy()/((double)vect->len-1);// len-1 because we only measure differences between numbers, always one less.
    Energy *= 0.5;
    candidate->Score[0]=Energy;// to do: find a scoring system whose max is 1.0
  }
  /* ********************************************************************** */
  void Print_Me() override {
    //printf("Tester_Mx_Loop class not implemented yet.\n");
  }
  /* ********************************************************************** */
  void Print_Org(OrgPtr candidate) override {
    printf("Top Org:\n");
    VectPtr vect = candidate->ray[0];
    //Energy = candidate->ray[0]->GetWaveEnergy();
    candidate->Print_Me();
    printf("\n");
    printf("Score0:%24.17g, Score1:%24.17g\n", candidate->Score[0], candidate->Score[1]);
  }
  /* ********************************************************************** */
  void Attach_StartingState(VectPtr StartingState0) override {
  }
};

/* ********************************************************************** */
class Tester_Echo;// forward
typedef Tester_Echo *Tester_EchoPtr;
typedef std::vector<Tester_EchoPtr> Tester_EchoVec;
class Tester_Echo : public Tester {// evolve to create a vector with max wave energy (max changes, as we define it)
public:
  const static uint32_t RunningStart = 50;//0;//100;//2000;
  const static uint32_t TestRuns = 100;
  int External_Node_Number=-1, Total_Node_Number=-1;// deliberately crashing values
  MatrixPtr model=nullptr;// behavior to imitate
  int MxWdt=-1, MxHgt=-1;
  int ModelIterations=10;
  /* ********************************************************************** */
  Tester_Echo(){
  }
  /* ********************************************************************** */
  ~Tester_Echo(){
    this->Clear_Model();
  }
  /* ********************************************************************** */
  void Clear_Model() {
    this->model = nullptr;
  }
  /* ********************************************************************** */
  void Generation_Start() override {// once per generation
  }
  /* ********************************************************************** */
  void Test(OrgPtr candidate) override {
    VectPtr InVect = candidate->ray[0];
    int NumCycles = InVect->len;
    Vect OutSignal(NumCycles);
    Vect ModelState(this->MxWdt);
    double EnergyIn = 0, EnergyOut = 0;
    int Iterations = 1;
    EnergyIn = InVect->GetWaveEnergy();
    double Score = 0;
    for (int CycleCnt=0; CycleCnt<NumCycles; CycleCnt++) {// CycleCnt could be TCnt instead
      ModelState.ray[0] = InVect->ray[CycleCnt];
      model->Iterate(&ModelState, Iterations, &ModelState);
      double SignalOut = ModelState.ray[0];
      OutSignal.ray[CycleCnt] = SignalOut;
    }
    EnergyOut = OutSignal.GetWaveEnergy();
    Score = EnergyOut - EnergyIn;// or EnergyOut/EnergyIn ?
    Score = ActFun(Score);// use sigmoid to compress into -1 to +1 range.
    candidate->Score[0]=Score;
  }
  /* ********************************************************************** */
  void Print_Me() override {
    //printf("Tester_Mx_Loop class not implemented yet.\n");
  }
  /* ********************************************************************** */
  void Print_Org(OrgPtr candidate) override {
    printf("Top Org:\n");
    VectPtr vect = candidate->ray[0];
    //Energy = candidate->ray[0]->GetWaveEnergy();
    candidate->Print_Me();
    printf("\n");
    printf("Score0:%24.17g, Score1:%24.17g\n", candidate->Score[0], candidate->Score[1]);
  }
  /* ********************************************************************** */
  void Profile_Model(MatrixPtr CurrentModel) override {
  }
  /* ********************************************************************** */
  void Attach_StartingState(VectPtr StartingState0) override {
  }
  /* ********************************************************************** */
  void Attach_Model(MatrixPtr model0) override {
    this->Clear_Model();
    this->model = model0;
    this->MxWdt=this->model->wdt; this->MxHgt=this->model->hgt;
    Total_Node_Number=this->MxWdt; External_Node_Number=1; // External_Node_Number is the number of sensorymotor (I/O) nodes in the model
  }
  /* ********************************************************************** */
  void Print_Echo(MatrixPtr candidate) {
    VectPtr InVect = candidate->ray[0];
    int NumCycles = InVect->len;
    Vect OutSignal(NumCycles);
    Vect ModelState(this->MxWdt);
    double EnergyIn = 0, EnergyOut = 0;
    int Iterations = 1;
    EnergyIn = InVect->GetWaveEnergy();
    double Score = 0;
    for (int CycleCnt=0; CycleCnt<NumCycles; CycleCnt++) {// CycleCnt could be TCnt instead
      double SignalIn = InVect->ray[CycleCnt];
      ModelState.ray[0] = SignalIn;
      model->Iterate(&ModelState, Iterations, &ModelState);
      double SignalOut = ModelState.ray[0];
      OutSignal.ray[CycleCnt] = SignalOut;
      printf("%f, %f\n", SignalIn, SignalOut);
    }
    EnergyOut = OutSignal.GetWaveEnergy();
    Score = EnergyOut - EnergyIn;// or EnergyOut/EnergyIn ?
    printf("EnergyIn:%f, EnergyOut:%f\n", EnergyIn, EnergyOut);
    //OutSignal.Print_Me();
  }
};

/* ********************************************************************** */
class Tester_Thread;// forward
typedef Tester_Thread *Tester_ThreadPtr;
typedef std::vector<Tester_ThreadPtr> Tester_ThreadVec;
class Tester_Thread : public Tester {// evolve to create a vector with max wave energy (max changes, as we define it)
public:
  std::atomic<int> Threads_Running;
  //int Threads_Running;
  /* ********************************************************************** */
  Tester_Thread(){
    this->Threads_Running = 0;
  }
  /* ********************************************************************** */
  ~Tester_Thread(){
  }
  /* ********************************************************************** */
  void Clear_Model() {
  }
  /* ********************************************************************** */
  void Generation_Start() override {// once per generation
  }
  /* ********************************************************************** */
  void Generation_Finish() override {// do all of our thread joining here
    //printf("Threads_Running:%i\n", this->Threads_Running);
    cout << "Threads_Running In:" << this->Threads_Running << endl;
    int bailcnt = 0;
    while (this->Threads_Running.load()>0){
      //printf("Threads_Running:%i\n", this->Threads_Running);
      cout << "Threads_Running:" << this->Threads_Running << " ";
      //std::this_thread::yield();
      if (bailcnt>200){
        cout << "Bail!" << endl;
        exit(1);
      }
      bailcnt++;
    }
    if (this->Threads_Running<0){
      cout << "Threads_Running:" << this->Threads_Running << "******************************************************\n";
      exit(1);
    }
    cout << "Threads_Running Out:" << this->Threads_Running << endl;
    //printf("Threads_Running Out:%i\n", this->Threads_Running);
  }
  /* ********************************************************************** */
  void Test(OrgPtr candidate) override {
    //printf("Inc_Threads:%i\n", this->Threads_Running);
    Inc_Threads();
    std::thread branch(&Tester_Thread::Test_Async, this, candidate);
    branch.detach();//branch.join(); // pauses until branch finishes
  }
  /* ********************************************************************** */
  void Test_Async(OrgPtr candidate) {
    //Inc_Threads();
    cout << "ta0." << "";
    VectPtr vect = candidate->ray[0];
    vect->Clip_Me(1.0);// hacky. here we modify the Org's genome in a test.
    double Range = vect->MaxLen();
    double Energy;
    Energy = vect->GetWaveEnergy()/((double)vect->len-1);// len-1 because we only measure differences between numbers, always one less.
    Energy *= 0.5;
    candidate->Score[0]=Energy;// to do: find a scoring system whose max is 1.0
    //cout << "Dec_Threads:" << this->Threads_Running << "\n";
    this->Dec_Threads();
    //scout << "Dec_Threads2:" << this->Threads_Running << "\n";
    cout << "ta1." << "";
  }
  /* ********************************************************************** */
  void Inc_Threads() {
    this->Threads_Running++;
  }
  /* ********************************************************************** */
  void Dec_Threads() {
    this->Threads_Running--;
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

#endif // TESTER_HPP_INCLUDED
