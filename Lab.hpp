#ifndef LAB_HPP_INCLUDED
#define LAB_HPP_INCLUDED
#include <iostream>

using namespace std;

#include "Tester_Sounder.hpp"
#include "Pop.hpp"

class Lab;// forward
typedef Lab *LabPtr;
class Lab {// Laboratory
public:
  /* ********************************************************************** */
  void Run_Test() {
    switch (5) {
    case 0:
      Test0();
      break;
    case 1:
      Test1();
      break;
    case 2:
      Test2();
      break;
    case 3:
      Test_Energy();
      break;
    case 4:
      Test_Thread();
      break;
    case 5:
      Test_Sounder();
      break;
    }
  }
  /* ********************************************************************** */
  void Test_Thread() {
    MatrixPtr TopOrg = nullptr;
    int OrgWdt = 64;//4;//16;//32;
    int OrgHgt = 1;
    printf("Test_Thread, OrgWdt:%i, OrgHgt:%i\n", OrgWdt, OrgHgt);
    Tester_ThreadPtr ThreadTest = new Tester_Thread();
    PopPtr pop = new Pop();
    pop->Assign_Params(100, OrgWdt, OrgHgt, ThreadTest, /* MaxOrgGens */ 10000, 1, /* EvoStagnationLimit */ 30000);
    pop->Evolve();
    TopOrg = pop->CloneTopOrg();
    delete pop;
    delete ThreadTest;
    delete TopOrg;
  }
  /* ********************************************************************** */
  void Test_Sounder() {
    MatrixPtr TopOrg = nullptr;
    Evo_Sounder(TopOrg);
    delete TopOrg;
  }
  /* ********************************************************************** */
  void Test_Energy() {
    MatrixPtr TopOrg = nullptr;
    Evo_Vect(TopOrg);
    delete TopOrg;
  }
  /* ********************************************************************** */
  void Test0() {
    MatrixPtr model = nullptr;
    VectPtr StartingState = nullptr;
    Evo_Model(model, StartingState);
    Evo_Mx(model, StartingState);
    delete model;
    delete StartingState;
  }
  /* ********************************************************************** */
  void Test1() {
    PopPtr pop = new Pop();
    TesterPtr tester = Init_Tester();
    pop->Attach_Tester(tester);// MUST create and attach a tester.
    pop->Evolve();
    delete pop;
    delete tester;
  }
  /* ********************************************************************** */
  void Test2() {
    MatrixPtr model = nullptr;
    VectPtr StartingState = new Vect();// to do: we need to define starting state length as equal to model width, as well as to fill it with - what?
    MatrixPtr TopOrg = nullptr;
    int ModelWdt = 8, ModelHgt = ModelWdt;
    printf("Echo Lab, ModelWdt:%i, ModelHgt:%i\n", ModelWdt, ModelHgt);
    Evo_Model(ModelWdt, model, StartingState);// ideally we want to pre-define StartingState as what?  zero vector?
    Evo_Echo(model, TopOrg);
    // next, we must print the winning org, but also the waveform that comes out when it is fed through the model.
    // we also want to compare retries to see how much they vary.
    delete model;
    delete StartingState;
    delete TopOrg;
  }
  /* ********************************************************************** */
  void Evo_Model(int ModelSize, MatrixPtr &BestModel, VectPtr &StartingState) {// Evolve a matrix model toward 'interestingness'.
    printf("Evo_Model, ModelSize:%i\n", ModelSize);
    Tester_Mx_WobblePtr wobbletester = new Tester_Mx_Wobble(ModelSize, ModelSize);
    if (StartingState == nullptr){// if no starting state, copy the default one and pass it outward.
      StartingState = wobbletester->StartingState->Clone_Me();
    } else {// if a starting state is predefined, use that.
      wobbletester->Attach_StartingState(StartingState);
    }
    PopPtr pop = new Pop();
    pop->Assign_Params(100, ModelSize, wobbletester, /* MaxOrgGens */ 1000, 1, /* EvoStagnationLimit */ 150);
    pop->Evolve();
    BestModel = pop->CloneTopOrg();//org->Copy_From(pop->GetTopOrg());
    delete pop;
    delete wobbletester;
  }
  /* ********************************************************************** */
  void Evo_Model(MatrixPtr &BestModel, VectPtr &StartingState) {// Evolve a matrix model toward 'interestingness'.
    int ModelSize = Org::DefaultWdt;
    Evo_Model(ModelSize, BestModel, StartingState);
  }
  /* ********************************************************************** */
  void Evo_Mx(MatrixPtr model, VectPtr StartingState) {
    int OrgSize = model->wdt;
    //OrgSize += 2;
    //OrgSize *= 2;

    PopStatsPtr StatPack = new PopStats();
    StatPack->Init();
    printf("Evo_Mx, OrgSize:%i\n", OrgSize);
    //printf("Model Sanity0:\n"); model->Print_Me();
    Tester_Mx_LoopPtr tester=new Tester_Mx_Loop(Org::DefaultWdt, Org::DefaultHgt);
    tester->Attach_StartingState(StartingState);
    tester->Attach_Model(model);
    printf("MxLoop StartingState:\n"); tester->StartingState->Print_Me();
    //printf("MxLoop Model Sanity1:\n"); tester->model->Print_Me();
    PopPtr pop = new Pop();
    int Retries = 100;//16;
    int EvoStagnationLimit = 1500;//200;//
    pop->Assign_Params(100, OrgSize, tester, /* MaxOrgGens */ 10000, Retries, /* EvoStagnationLimit */ EvoStagnationLimit);
    //printf("Model Sanity2:\n"); ((Tester_Mx_LoopPtr)(pop->tester))->model->Print_Me();
    pop->Attach_Stats(StatPack);// error, first sample is not initialized.
    pop->Evolve();
    printf("\n"); StatPack->Print_Me();
    delete pop;
    delete tester;
    delete StatPack;
  }
  /* ********************************************************************** */
  void Evo_Sounder(MatrixPtr &TopOrg) {// Evolve a vector to be the highest-energy wave
    int OrgWdt = 64;//4;//16;//32;
    int OrgHgt = 1;
    int PopSize = 100;
    printf("Evo_Sounder, OrgWdt:%i, OrgHgt:%i\n", OrgWdt, OrgHgt);
    Tester_SounderPtr SoundTester = new Tester_Sounder();
    SoundTester->Assign_Pop_Size(PopSize, OrgWdt);
    PopPtr pop = new Pop();
    pop->Assign_Params(PopSize, OrgWdt, OrgHgt, SoundTester, /* MaxOrgGens */ 10000, 1, /* EvoStagnationLimit */ 30000);
    pop->Evolve();
    TopOrg = pop->CloneTopOrg();
    delete pop;
    delete SoundTester;
  }
  /* ********************************************************************** */
  void Evo_Vect(MatrixPtr &TopOrg) {// Evolve a vector to be the highest-energy wave
    int OrgWdt = 64;//4;//16;//32;
    int OrgHgt = 1;
    printf("Evo_Vect, OrgWdt:%i, OrgHgt:%i\n", OrgWdt, OrgHgt);
    Tester_VectPtr vectester = new Tester_Vect();
    PopPtr pop = new Pop();
    pop->Assign_Params(100, OrgWdt, OrgHgt, vectester, /* MaxOrgGens */ 10000, 1, /* EvoStagnationLimit */ 30000);
    pop->Evolve();
    TopOrg = pop->CloneTopOrg();
    delete pop;
    delete vectester;
  }
  /* ********************************************************************** */
  void Evo_Echo(MatrixPtr model, MatrixPtr &TopOrg) {// Evolve a wave vector to evoke the highest-energy wave from a model
    int OrgWdt = 64;//4;//16;//32;
    int OrgHgt = 1;
    int ModelWdt = model->wdt, ModelHgt = ModelWdt;
    printf("Evo_Echo, OrgWdt:%i, OrgHgt:%i, ModelWdt:%i, ModelHgt:%i\n", OrgWdt, OrgHgt, ModelWdt, ModelHgt);
    //printf("Evo_Echo, OrgWdt:%i, OrgHgt:%i\n", OrgWdt, OrgHgt);
    Tester_EchoPtr vectester = new Tester_Echo();
    vectester->Attach_Model(model);
    PopPtr pop = new Pop();
    pop->Assign_Params(100, OrgWdt, OrgHgt, vectester, /* MaxOrgGens */ 10000, 1, /* EvoStagnationLimit */ 30000);
    pop->Evolve();
    TopOrg = pop->CloneTopOrg();
    printf("Print_Echo\n");
    vectester->Print_Echo(TopOrg);
    delete pop;
    delete vectester;
  }
  /* ********************************************************************** */
  TesterPtr Init_Tester() {
    TesterPtr tester = nullptr;
    switch (1) {
    case 0:
      tester=new Tester_Mx(Org::DefaultWdt, Org::DefaultHgt);
      break;
    case 1:
      tester=new Tester_Net();
      break;
    case 2:
      //tester=new Tester_Mx_Loop(Org::DefaultWdt-2, Org::DefaultHgt-2);
      tester=new Tester_Mx_Loop(Org::DefaultWdt, Org::DefaultHgt);
      break;
    case 3:
      tester = new Tester_Mx_Wobble(Org::DefaultWdt, Org::DefaultHgt);
      break;
    default:break;
    }
    return tester;
  }
  /* ********************************************************************** */
  void Big_Survey(MatrixPtr model, VectPtr StartingState) {
    /*
    Next:
    create a 'package' of instances executed with the same parameters to statistically score those parameters.  How many instances?  50? 100?

    margin of error = 1/sqrt(popsize)

    1/(400^0.5) = 0.5, so if score is 90%, we would be 95% certain the results are between 85% and 95%.

    generate models;
    tester = new Tester_Mx_Loop();
    pop.Attach(tester);
    for OrgSize (8 and 16) {
      StatObj = new Stats();
      pop.SetOrgSize(orgsize);
      for all Models {
        tester.Attach(model);
        for each sample {// do this about 400 times. each time is a new evolution with all the same parameters
          pop.Reset();
          pop.Evolve();// here is all the same tester, model, and parameters.
          pop.Get_Stats(&StatObj);
        }
      }
      StatObj.Print_Me();Evo_Mx
    }// orgsize

    */
  }
};

#endif // LAB_HPP_INCLUDED


/*
Experiments to do:
see if an mx loop with more nodes than the model is any better than otherwise.  requires un-binding various vect and matrix sizes from org::default.
make a stats framework that scores hundreds of runs of a particular test, that can be compared with other tests with different parameters.
*/

/*
goal is to breed models for interestingness,
then feed the winner(s) to the tester, and feed the tester to the population
then the pop will evolve against interesting models.

a pop must take an arbitrary tester as a parameter.

*/
