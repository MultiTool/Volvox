#ifndef POP_HPP_INCLUDED
#define POP_HPP_INCLUDED

#include "Org.hpp"
#include "Tester.hpp"

//#define popmax 1000
#define popmax 100
//#define popmax 10

/* ********************************************************************** */
class Pop;
typedef Pop *PopPtr;
class Pop {
public:
  uint32_t popsz;
  OrgVec forest;
  OrgVec ScoreDexv; // for sorting

  double avgnumwinners = 0.0;
  TesterPtr tester;// crucible
  uint32_t GenCnt;
  const double MutRate=0.2;//0.2;//0.3;//0.8
  double SurvivalRate=0.2;//0.5;
  size_t NumSurvivors;
  double SumScores=0,AvgScore=0.0;
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
    if (true){
      tester=new TesterMx(Org::DefaultWdt, Org::DefaultHgt);
    }else{
      tester=new TesterNet();
    }
    this->GenCnt=0;
    NumSurvivors = popsize * SurvivalRate;
    SumScores=0;
  }
  /* ********************************************************************** */
  void Evolve() {// evolve for generations
    uintmax_t EvoStagnationLimit = 75;//50;
    for (int RetryCnt=0;RetryCnt<16;RetryCnt++){
      double CurrentTopScore, TopScore = 0.0;
      int AbortCnt=0;
      for (int gcnt=0;gcnt<1000;gcnt++){
        this->Gen();
        CurrentTopScore=this->GetTopScore();
        if (TopScore<CurrentTopScore){
          AbortCnt=0; TopScore=CurrentTopScore;
        }else{
          AbortCnt++; // stopping condition: if best score hasn't improved in 50 generations, bail.
          if (AbortCnt>EvoStagnationLimit){ break; }
        }
      }
      this->Print_Results();
      for (int gcnt=0;gcnt<50;gcnt++){
        this->Gen_No_Mutate();// coast, no mutations
      }
      this->Print_Results();
      printf("RetryCnt:%i\n\n", RetryCnt);
      //std::cin.getline(name,256);

      this->Restart();
    }
  }
  /* ********************************************************************** */
  double GetTopScore() {// not working yet
    OrgPtr TopOrg = ScoreDexv[0];
    double TopScore = TopOrg->Score[0];
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
    this->GenCnt=0; SumScores=0.0; AvgScore=0.0;
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
  void Gen() { // each generation
    this->Gen_No_Mutate();
    this->Mutate(MutRate, MutRate);
  }
  /* ********************************************************************** */
  void Gen_No_Mutate() { // call this by itself to 'coast', reproduce and winnow generations without mutation.
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
    OrgPtr TopOrg = ScoreDexv[0];
    double TopScore = TopOrg->Score[0];
    double TopDigiScore = TopOrg->Score[1];
    Birth_And_Death();
    SumScores+=TopDigiScore;
    //AvgScore=SumScores/this->GenCnt;
    AvgScore=(AvgScore*0.9) + (TopDigiScore*0.1);
    //printf("GenCnt:%i, TopScore:%f, AvgScore:%f, TopDigiScore::%f\n", this->GenCnt, TopScore, AvgScore, TopDigiScore);
    this->GenCnt++;
  }
  /* ********************************************************************** */
  void Print_Results() {
    printf("Print_Results\n");
    OrgPtr TopOrg = ScoreDexv[0];

    if (false){
      printf("Model Matrix\n");
      tester->Print_Me();
      printf("Top Matrix\n");
      TopOrg->Print_Me();
    }

    double TopScore = TopOrg->Score[0];
    double TopDigiScore = TopOrg->Score[1];
    AvgScore=(AvgScore*0.9) + (TopDigiScore*0.1);
    printf("GenCnt:%i, TopScore:%f, AvgScore:%f, TopDigiScore::%f\n", this->GenCnt, TopScore, AvgScore, TopDigiScore);
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
    std::sort (ScoreDexv.begin(), ScoreDexv.end(), DescendingScore);
  }
  /* ********************************************************************** */
  void Birth_And_Death() {
    size_t siz = ScoreDexv.size();
    size_t topcnt, cnt;
    OrgPtr doomed, child;
    topcnt = 0;
    for (cnt=NumSurvivors; cnt<siz; cnt++) {
      doomed = ScoreDexv[cnt]; doomed->Doomed = true;
      delete doomed;
      child = ScoreDexv[topcnt]->Spawn();// Whenever one dies, replace it with the child of another.
      ScoreDexv[cnt] = child;
      if (++topcnt>=NumSurvivors) {topcnt=0;}
    }
  }
  /* ********************************************************************** */
  void Mutate_Sorted(double Pop_MRate, double Org_MRate) {
    size_t siz = this->ScoreDexv.size();
    for (size_t cnt=16; cnt<siz; cnt++) {
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

