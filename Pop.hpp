#ifndef POP_HPP_INCLUDED
#define POP_HPP_INCLUDED

#include "Org.hpp"
#include "Tester.hpp"

//#define popmax 1000
#define popmax 100
//#define popmax 20
//#define popmax 10
//#define popmax 5
//#define popmax 2

/* ********************************************************************** */
class Pop;// forward
typedef Pop *PopPtr;
class Pop {
public:
  uint32_t popsz;
  OrgVec Forest; // for sorting

  TesterPtr tester;// crucible
  TesterMxWobblePtr wobble = nullptr;
  uint32_t GenCnt;
  const double SurvivalRate=0.2;//0.02;//0.05;//0.5;
  const double MutRate=0.2;//0.5;//0.3;//0.8//0.6;//0.99;//
  int MaxOrgGens = 10000;//10000;//1000000;//50;//
  int MaxRetries = 1;//4;//2;//16;
  uintmax_t EvoStagnationLimit = 1500;//16384;//3000;// 5000;//100;//75;//50;
  size_t NumSurvivors;
  double SumScores=0, AvgTopDigi=0.0;
  double AllTimeTopScore=0.0;
  double CurrentTopScore=0.0;
  /* ********************************************************************** */
  Pop() : Pop(popmax) {
  }
  /* ********************************************************************** */
  Pop(int popsize) {
    this->Init(popsize);
  }
  /* ********************************************************************** */
  ~Pop() {
    this->Clear();
  }
  /* ********************************************************************** */
  void Assign_Params(int popsize0, TesterPtr tester0, int MaxOrgGens0, int MaxRetries0, uintmax_t EvoStagnationLimit0) {
    this->Clear();// note: clear deletes the previous tester, so this is bad.  decide if tester is created and owned externally, or maybe just an enum parameter.
    this->tester = tester0;
    this->MaxOrgGens = MaxOrgGens0;
    this->MaxRetries = MaxRetries0;
    this->EvoStagnationLimit = EvoStagnationLimit0;
    this->InitPop(popsize0);
  }
  /*
goal is to breed models for interestingness,
then feed the winner(s) to the tester, and feed the tester to the population
then the pop will evolve against interesting models.

so main pop has a subpop to breed models, and the best model is cloned and passed to the any tester that can use an mx model.
requires: intermediate subclass of tester that takes mx models.

or, code that holds both pops, the model breeder and the network breeder.
in that case the network breeder will take the winner model as a param, then pass it to its tester as a param.
more complicated than first option.

but either way, a pop must either take an arbitrary tester as a parameter, or maybe inherit from main pop and override tester creating part.
if override tester creator, we create and delete tester internally.

  */
  /* ********************************************************************** */
  void Init(int popsize) {// is it really necessary to be able to re-init without just deleting the population?

    switch (3){
    case 0:
      tester=new TesterMx(Org::DefaultWdt, Org::DefaultHgt);
      break;
    case 1:
      tester=new TesterNet();
      break;
    case 2:
      //tester=new TesterMxLoop(Org::DefaultWdt-2, Org::DefaultHgt-2);
      tester=new TesterMxLoop(Org::DefaultWdt, Org::DefaultHgt);
      break;
    case 3:
      tester = wobble = new TesterMxWobble(Org::DefaultWdt, Org::DefaultHgt);
      break;
    default:break;
    }

    this->InitPop(popsize);
  }
  /* ********************************************************************** */
  void InitPop(int popsize) {// Create and seed the population of creatures.
    Org *org;
    int pcnt;
    this->popsz = popsize;
    Forest.resize(popsize);
    for (pcnt=0; pcnt<popsize; pcnt++) {
      org = Org::Abiogenate();
      Forest.at(pcnt) = org;
    }
    NumSurvivors = popsize * SurvivalRate;
  }
  /*
We want this instead:
mutate children
test, score, sort
report (print)
reproduce (birth and death)

or this?
test, score, sort
report (print)
reproduce (birth and death)
mutate children
  */
  /* ********************************************************************** */
  void Evolve() {// evolve for generations
    for (int RetryCnt=0;RetryCnt<MaxRetries;RetryCnt++){
      this->Init_Evolution();
      double CurrentTopScoreLocal;
      AllTimeTopScore = 0.0;
      int AbortCnt=0;
      printf("\nRetryCnt:%i\n", RetryCnt);
      //printf("Gen:%i\n", GenCnt);
      for (int gcnt=0;gcnt<MaxOrgGens;gcnt++){
        //printf("Gen:%i\n", GenCnt);
        this->Gen();
        CurrentTopScoreLocal=this->GetTopScore();
        if (CurrentTopScoreLocal>=1.0){ break; }
        if (AllTimeTopScore<CurrentTopScoreLocal){
          AbortCnt=0; AllTimeTopScore=CurrentTopScoreLocal;
        }else{
          AbortCnt++; // stopping condition: if best score hasn't improved in EvoStagnationLimit generations, bail.
          if (AbortCnt>EvoStagnationLimit){ printf("Stagnated.\n"); break; }
        }
      }
      this->Print_Results();
      if (false){
        for (int gcnt=0;gcnt<50;gcnt++){
          this->Gen_No_Mutate();// coast, no mutations
        }
        this->Print_Results();
      }
      OrgPtr TopOrg = this->GetTopOrg();
      tester->Print_Org(TopOrg);
      //std::cin.getline(name,256);
    }
  }
  /* ********************************************************************** */
  void Gen() { // each generation
    this->Gen_No_Mutate();
    this->Mutate(MutRate, MutRate);
  }
  /* ********************************************************************** */
  void Gen_No_Mutate() { // call this by itself to 'coast', reproduce and winnow generations without mutation.
    Score_And_Sort();
    Collect_Stats();
    Birth_And_Death();
    this->GenCnt++;
  }
  /* ********************************************************************** */
  void Score_And_Sort() {
    uint32_t popsize = this->Forest.size();
    OrgPtr candidate;
    if (false){
      tester->Reset_Input();
    }
    for (uint32_t pcnt=0; pcnt<popsize; pcnt++) {
      candidate = Forest[pcnt];
      tester->Test(candidate);
    }
    Sort();
  }
  /* ********************************************************************** */
  void Collect_Stats() {
    OrgPtr TopOrg = this->GetTopOrg();
    double PrevTopScore = CurrentTopScore;
    CurrentTopScore = TopOrg->Score[0];
    double TopDigiScore = TopOrg->Score[1]; SumScores+=TopDigiScore;
    double ModelStateMag = TopOrg->ModelStateMag;

    //AvgTopDigi=SumScores/this->GenCnt;
    AvgTopDigi=(AvgTopDigi*0.9) + (TopDigiScore*0.1);
    //if (this->GenCnt % 1 == 0){
    if (this->CurrentTopScore != PrevTopScore){
      //printf("GenCnt:%4d, CurrentTopScore:%f, AvgTopDigi:%f, TopDigiScore::%f\n", this->GenCnt, CurrentTopScore, AvgTopDigi, TopDigiScore);
      //printf("GenCnt:%4d, CurrentTopScore:%f, TopDigiScore:%f\n", this->GenCnt, CurrentTopScore, TopDigiScore);
      //printf("GenCnt:%4d, TopScore:%24.17g, TopDigiScore:%f, ModelStateMag:%f\n", this->GenCnt, CurrentTopScore, TopDigiScore, ModelStateMag);
      //printf("GenCnt:%4d, TopScore0:%24.17g, TopScore1:%24.17g\n", this->GenCnt, CurrentTopScore, TopDigiScore);// full resolution of double
      //printf("GenCnt:%4d, TopScore0:%1.20g, TopScore1:%1.20g\n", this->GenCnt, CurrentTopScore, TopDigiScore);
      printf("GenCnt:%4d, ", this->GenCnt);
      TopOrg->Print_Scores();
      printf("\n");
    }
  }
  /* ********************************************************************** */
  void Print_Results() {
    //printf("Print_Results\n");
    OrgPtr TopOrg = Forest[0];

    if (false){
      printf("Model Matrix\n");
      tester->Print_Me();
      printf("Top Matrix\n");
      TopOrg->Print_Me();
    }

    double TopScore = TopOrg->Score[0];
    double TopDigiScore = TopOrg->Score[1];
    AvgTopDigi=(AvgTopDigi*0.9) + (TopDigiScore*0.1);
    //printf("GenCnt:%i, TopScore:%f, AvgTopDigi:%f, TopDigiScore:%f\n", this->GenCnt, TopScore, AvgTopDigi, TopDigiScore);
    //printf("GenCnt:%4d, TopScore0:%24.17g, TopScore1:%24.17g\n", this->GenCnt, TopScore, TopDigiScore);// full resolution of double
    printf("GenCnt:%4d, TopScore0:%1.20g, TopScore1:%1.20g\n", this->GenCnt, CurrentTopScore, TopDigiScore);
  }
  /* ********************************************************************** */
  OrgPtr CloneTopOrg() {// deliver copy of top org that will outlive this whole population instance.
    return this->GetTopOrg()->Spawn();
  }
  /* ********************************************************************** */
  OrgPtr GetTopOrg() {
    OrgPtr TopOrg = Forest[0];
    return TopOrg;
  }
  /* ********************************************************************** */
  double GetTopScore() {
    OrgPtr TopOrg = this->GetTopOrg();
    double TopScore = TopOrg->Score[0];
    return TopScore;
  }
  /* ********************************************************************** */
  void Init_Evolution() {// re-initialize the population genome without changing the tester or the test
    Org *org;
    this->GenCnt=0; SumScores=0.0; AvgTopDigi=0.0;
    AllTimeTopScore=0.0; CurrentTopScore=0.0;
    size_t pcnt, popsize = Forest.size();
    for (pcnt=0; pcnt<popsize; pcnt++) {
      org = Forest.at(pcnt);
      org->Rand_Init();
    }
  }
  /* ********************************************************************** */
  void Clear() {// is it really necessary to be able to clear without just deleting the population?
    size_t siz, pcnt;
    siz = Forest.size();
    for (pcnt=0; pcnt<siz; pcnt++) {
      delete Forest.at(pcnt);
    }
    delete tester;
  }
  /* ********************************************************************** */
  double AvgBeast() {
    size_t siz = Forest.size();
    double sum = 0.0;
    for (size_t cnt=0; cnt<siz; cnt++) {
      sum += Forest[cnt]->Score[0];
    }
    sum /= (double)siz;
    return sum;
  }
  /* ********************************************************************** */
  void Sort() {
    std::random_shuffle(Forest.begin(), Forest.end());//
    std::sort(Forest.begin(), Forest.end(), DescendingScore);
  }
  static bool AscendingScore(OrgPtr b0, OrgPtr b1) {
    return b0->Compare_Score(b1) > 0;
  }
  static bool DescendingScore(OrgPtr b0, OrgPtr b1) {
    return b1->Compare_Score(b0) > 0;
  }
  /* ********************************************************************** */
  void Birth_And_Death() {
    size_t siz = Forest.size();
    size_t topcnt, cnt;
    OrgPtr doomed, child, survivor;
    topcnt = 0;
    for (cnt=0; cnt<NumSurvivors; cnt++) {
      survivor = Forest[cnt];
      survivor->Reset();
    }
    for (cnt=NumSurvivors; cnt<siz; cnt++) {
      doomed = Forest[cnt]; doomed->Doomed = true;
      delete doomed;
      child = Forest[topcnt]->Spawn();// Whenever one dies, replace it with the child of another.
      Forest[cnt] = child;
      if (++topcnt>=NumSurvivors) {topcnt=0;}
    }
  }
  /* ********************************************************************** */
  void Mutate(double Pop_MRate, double Org_MRate) {
    OrgPtr org;
    size_t LastOrg;
    size_t siz = this->Forest.size(); LastOrg = siz-1;
    for (size_t cnt=this->NumSurvivors; cnt<LastOrg; cnt++) {
      //if (frand()<Pop_MRate) {
      org = this->Forest[cnt];
      org->Mutate_Me(Org_MRate);
      //}
    }
    org = this->Forest[LastOrg];// very last mutant is 100% randomized, to introduce 'new blood'
    org->Rand_Init();
  }
};

#endif // POP_HPP_INCLUDED

