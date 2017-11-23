#ifndef TESTER_HPP_INCLUDED
#define TESTER_HPP_INCLUDED

#include "Org.hpp"
// #include "Model.hpp"
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
class TesterMxLoop;// forward
typedef TesterMxLoop *TesterMxLoopPtr;
typedef std::vector<TesterMxLoopPtr> TesterMxLoopVec;
class TesterMxLoop : public Tester {// evolve to create a mirror of a continuous behavior
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
  TesterMxLoop(int MxWdt0, int MxHgt0){
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
  ~TesterMxLoop(){
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
  void Reset_Input() override {// once per generation
    this->StartingState->Rand_Init();
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
    //printf("TesterMxLoop class not implemented yet.\n");
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
    this->StartingState->Copy_From(StartingState0);
  }
  /* ********************************************************************** */
  void Attach_Model(MatrixPtr model0) override {
    this->Clear_Model();
    this->model = model0;
    this->MxWdt=this->model->wdt; this->MxHgt=this->model->hgt;
    External_Node_Number=this->MxWdt/2; Total_Node_Number=this->MxWdt;
    Profile_Model(this->model);
  }
};

/* ********************************************************************** */
class TesterMxWobble;// forward
typedef TesterMxWobble *TesterMxWobblePtr;
typedef std::vector<TesterMxWobblePtr> TesterMxWobbleVec;
class TesterMxWobble : public Tester {// evolve to create models with greater variance in amplitude, to be more interesting
public:
  const static uint32_t RunningStart = 50;//0;//100;//2000;
  const static uint32_t TestRuns = 100;// 10
  int Num_Bins = 16, Max_Bin = Num_Bins-1;
  int ModelIterations=1;
  int MxWdt, MxHgt;
  VectPtr StartingState;
  /* ********************************************************************** */
  TesterMxWobble(int MxWdt0, int MxHgt0){
    this->MxWdt=MxHgt0; this->MxHgt=MxHgt0;
    StartingState = new Vect(MxWdt0);
    StartingState->Rand_Init();
  }
  /* ********************************************************************** */
  ~TesterMxWobble(){
    delete StartingState;
  }
  /* ********************************************************************** */
  void Reset_Input() override {// once per generation
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
    //printf("TesterMxLoop class not implemented yet.\n");
  }
  /* ********************************************************************** */
  void Print_Org(OrgPtr candidate) override {
    printf("Top Org:\n");
    candidate->Print_Me();
    printf("\n");
    int Bins[Num_Bins] = {};
    Vect ModelState(this->MxWdt);
    ModelState.Copy_From(StartingState);
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
};

/* ********************************************************************** */
class TesterNet;// forward
typedef TesterNet *TesterNetPtr;
typedef std::vector<TesterNetPtr> TesterNetVec;
class TesterNet : public Tester {// evolve to create a backpropagation learning rule
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
  VectPtr ModelStateSeed;//Total_Node_Number);
  /* ********************************************************************** */
  TesterNet(){
    printf("PerfectDigi:%f, HCubeDims:%i\n", PerfectDigi, HCubeDims);
    ModelStateSeed = new Vect(Total_Node_Number);
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
  ~TesterNet(){
    delete ModelStateSeed;
    Delete_Models();
    delete MacroNet;
  }
  /* ********************************************************************** */
  void Reset_Input() override {// once per generation
    Scramble_Models();
    this->Scramble_ModelStateSeed();
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
      ModelState.Copy_From(ModelStateSeed);
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
    //printf("TesterNet class not implemented yet.\n");
  }
  /* ********************************************************************** */
  void Print_Org(OrgPtr candidate) override {
  }
  /* ********************************************************************** */
  void Profile_Model(MatrixPtr CurrentModel) override {
    int Num_Bins = 16;
    int Bins[Num_Bins] = {};
    Vect ModelState(Total_Node_Number);
    ModelState.Copy_From(ModelStateSeed);
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
      ModelStateSeed->Rand_Init();// mutate 100%
      Mag = ModelStateSeed->Magnitude();
      printf("Seed Mag:%f\n", Mag);
    } while (Mag<1.0);
  }
};

#endif // TESTER_HPP_INCLUDED
