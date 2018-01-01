#ifndef LAB_HPP_INCLUDED
#define LAB_HPP_INCLUDED
#include <iostream>

using namespace std;

#include "Pop.hpp"

class Lab;// forward
typedef Lab *LabPtr;
class Lab {// Laboratory
public:
  /* ********************************************************************** */
  void Run_Test() {
    MatrixPtr model = nullptr;
    VectPtr StartingState = nullptr;
    Evo_Model(model, StartingState);
    Evo_Mx(model, StartingState);
    delete model;
    delete StartingState;
  }
  /* ********************************************************************** */
  void Run_Test2() {
    PopPtr pop = new Pop();
    TesterPtr tester = Init_Tester();
    pop->Attach_Tester(tester);// MUST create and attach a tester.
    pop->Evolve();
    delete pop;
    delete tester;
  }
  /* ********************************************************************** */
  void Evo_Model(MatrixPtr &model, VectPtr &StartingState) {// Evolve a matrix model toward 'interestingness'.
    int ModelSize = Org::DefaultWdt;
    printf("Evo_Model, ModelSize:%i\n", ModelSize);
    TesterMxWobblePtr wobbletester = new TesterMxWobble(Org::DefaultWdt, Org::DefaultHgt);
    //VectPtr StartingState;
    PopPtr pop = new Pop();
    pop->Assign_Params(100, ModelSize, wobbletester, /* MaxOrgGens */ 1000, 1, /* EvoStagnationLimit */ 150);
    pop->Evolve();
    StartingState = wobbletester->StartingState->Clone_Me();
    model = pop->CloneTopOrg();//org->Copy_From(pop->GetTopOrg());
    delete pop;
    delete wobbletester;
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
    TesterMxLoopPtr tester=new TesterMxLoop(Org::DefaultWdt, Org::DefaultHgt);
    tester->Attach_StartingState(StartingState);
    tester->Attach_Model(model);
    printf("MxLoop StartingState:\n"); tester->StartingState->Print_Me();
    //printf("MxLoop Model Sanity1:\n"); tester->model->Print_Me();
    PopPtr pop = new Pop();
    int Retries = 100;//16;
    int EvoStagnationLimit = 1500;//200;//
    pop->Assign_Params(100, OrgSize, tester, /* MaxOrgGens */ 10000, Retries, /* EvoStagnationLimit */ EvoStagnationLimit);
    //printf("Model Sanity2:\n"); ((TesterMxLoopPtr)(pop->tester))->model->Print_Me();
    pop->Attach_Stats(StatPack);// error, first sample is not initialized.
    pop->Evolve();
    printf("\n"); StatPack->Print_Me();
    delete pop;
    delete tester;
    delete StatPack;
  }
  /* ********************************************************************** */
  TesterPtr Init_Tester() {
    TesterPtr tester = nullptr;
    switch (1){
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
      tester = new TesterMxWobble(Org::DefaultWdt, Org::DefaultHgt);
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
tester = new TesterMxLoop();
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
