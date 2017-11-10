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
  OrgVec forest;
  OrgVec ScoreDexv; // for sorting

  double avgnumwinners = 0.0;
  TesterPtr tester;// crucible
  TesterMxWobblePtr wobble = nullptr;
  uint32_t GenCnt;
  const double SurvivalRate=0.2;//0.02;//0.05;//0.5;
  const double MutRate=0.2;//0.5;//0.3;//0.8//0.6;//0.99;//
  const int MaxOrgGens = 10000;//10000;
  const int MaxRetries = 1;//16;
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
  void Init(int popsize) {// is it really necessary to be able to re-init without just deleting the population?
    Org *org;
    int pcnt;
    this->popsz = popsize;
    forest.resize(popsize);
    ScoreDexv.resize(popsize);
    for (pcnt=0; pcnt<popsize; pcnt++) {
      org = Org::Abiogenate();
      ScoreDexv.at(pcnt) = org;
    }
    switch (1){
    case 0:
      tester=new TesterMx(Org::DefaultWdt, Org::DefaultHgt);
      break;
    case 1:
      tester=new TesterNet();
      break;
    case 2:
      tester=new TesterMxLoop(Org::DefaultWdt, Org::DefaultHgt);
      break;
    case 3:
      tester = wobble = new TesterMxWobble(Org::DefaultWdt, Org::DefaultHgt);
      break;
    default:break;
    }

    this->GenCnt=0;
    NumSurvivors = popsize * SurvivalRate;
    SumScores=0;
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
    uintmax_t EvoStagnationLimit = 1500;//5000;//100;//75;//50;
    for (int RetryCnt=0;RetryCnt<MaxRetries;RetryCnt++){
      double CurrentTopScoreLocal;
      AllTimeTopScore = 0.0;
      int AbortCnt=0;
      for (int gcnt=0;gcnt<MaxOrgGens;gcnt++){
        this->Gen();
        CurrentTopScoreLocal=this->GetTopScore();
        if (AllTimeTopScore<CurrentTopScoreLocal){
          AbortCnt=0; AllTimeTopScore=CurrentTopScoreLocal;
        }else{
          AbortCnt++; // stopping condition: if best score hasn't improved in EvoStagnationLimit generations, bail.
          if (AbortCnt>EvoStagnationLimit){ break; }
        }
      }
      this->Print_Results();
      if (false){
        for (int gcnt=0;gcnt<50;gcnt++){
          this->Gen_No_Mutate();// coast, no mutations
        }
        this->Print_Results();
      }
      printf("RetryCnt:%i\n\n", RetryCnt);
      OrgPtr TopOrg = this->GetTopOrg();
      if (wobble!=nullptr){
        wobble->Print_Org(TopOrg);
      }
      //std::cin.getline(name,256);

      this->Restart();
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
    uint32_t popsize = this->ScoreDexv.size();
    OrgPtr candidate;
    if (false){
      tester->Reset_Input();
    }
    for (uint32_t pcnt=0; pcnt<popsize; pcnt++) {
      candidate = ScoreDexv[pcnt];
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
      printf("GenCnt:%4d, TopScore:%24.17g, TopDigiScore:%f\n", this->GenCnt, CurrentTopScore, TopDigiScore);
    }
  }
  /* ********************************************************************** */
  void Print_Results() {
    //printf("Print_Results\n");
    OrgPtr TopOrg = ScoreDexv[0];

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
    printf("GenCnt:%4d, TopScore:%24.17g, TopDigiScore:%f\n", this->GenCnt, TopScore, TopDigiScore);
  }
  /* ********************************************************************** */
  OrgPtr GetTopOrg() {
    OrgPtr TopOrg = ScoreDexv[0];
    return TopOrg;
  }
  /* ********************************************************************** */
  double GetTopScore() {
    OrgPtr TopOrg = this->GetTopOrg();
    double TopScore = TopOrg->Score[0];
    if (TopScore>1.0){
      //printf("Pop error:%f", TopScore);
    }
    return TopScore;
  }
  /* ********************************************************************** */
  void Restart() {// re-initialize the population genome without changing the tester or the test
    Org *org;
    size_t pcnt, popsize = ScoreDexv.size();
    for (pcnt=0; pcnt<popsize; pcnt++) {
      org = ScoreDexv.at(pcnt);
      org->Rand_Init();
    }
    this->GenCnt=0; SumScores=0.0; AvgTopDigi=0.0;
  }
  /* ********************************************************************** */
  void Clear() {// is it really necessary to be able to clear without just deleting the population?
    size_t siz, pcnt;
    siz = ScoreDexv.size();
    for (pcnt=0; pcnt<siz; pcnt++) {
      delete ScoreDexv.at(pcnt);
    }
    delete tester;
  }
  /* ********************************************************************** */
  double AvgBeast() {
    size_t siz = ScoreDexv.size();
    double sum = 0.0;
    for (size_t cnt=0; cnt<siz; cnt++) {
      sum += ScoreDexv[cnt]->Score[0];
    }
    sum /= (double)siz;
    return sum;
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
    std::sort(ScoreDexv.begin(), ScoreDexv.end(), DescendingScore);
  }
  /* ********************************************************************** */
  void Birth_And_Death() {
    size_t siz = ScoreDexv.size();
    size_t topcnt, cnt;
    OrgPtr doomed, child, survivor;
    topcnt = 0;
    for (cnt=0; cnt<NumSurvivors; cnt++) {
      survivor = ScoreDexv[cnt];
      survivor->Reset();
    }
    for (cnt=NumSurvivors; cnt<siz; cnt++) {
      doomed = ScoreDexv[cnt]; doomed->Doomed = true;
      delete doomed;
      child = ScoreDexv[topcnt]->Spawn();// Whenever one dies, replace it with the child of another.
      ScoreDexv[cnt] = child;
      if (++topcnt>=NumSurvivors) {topcnt=0;}
    }
  }
  /* ********************************************************************** */
  void Mutate(double Pop_MRate, double Org_MRate) {
    OrgPtr org;
    size_t LastOrg;
    size_t siz = this->ScoreDexv.size(); LastOrg = siz-1;
    for (size_t cnt=this->NumSurvivors; cnt<LastOrg; cnt++) {
      //if (frand()<Pop_MRate) {
      org = this->ScoreDexv[cnt];
      org->Mutate_Me(Org_MRate);
      //}
    }
    org = this->ScoreDexv[LastOrg];// very last mutant is 100% randomized, to introduce 'new blood'
    org->Rand_Init();
  }
};

#endif // POP_HPP_INCLUDED

