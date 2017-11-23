#include <iostream>

using namespace std;

#include "Node.hpp"
#include "Cluster.hpp"
#include "Pop.hpp"

/*
ok so
wobbletester to
pop.evolve
org = pop.bestorg; // NOT a model.  or could wobbletester's override output models?

*/
  /* ********************************************************************** */
  MatrixPtr Evo_Model(MatrixPtr &model, VectPtr &StartingState) {// to do: move to outside of pop class
    printf("Evo_Model\n");
    //MatrixPtr org;// = new Matrix();
    TesterMxWobblePtr wobbletester = new TesterMxWobble(Org::DefaultWdt, Org::DefaultHgt);
    //VectPtr StartingState;
    PopPtr pop = new Pop();
    pop->Assign_Params(100, wobbletester, /* MaxOrgGens */ 1000, 1, /* EvoStagnationLimit */ 150);
    pop->Evolve();
    StartingState = wobbletester->StartingState->Clone_Me();
    model = pop->CloneTopOrg();//org->Copy_From(pop->GetTopOrg());
    delete pop;
    delete wobbletester;
    return model;
  }
  /* ********************************************************************** */
  void Evo_Mx(MatrixPtr model, VectPtr StartingState) {
    printf("Evo_Mx\n");
    //printf("Model Sanity0:\n"); model->Print_Me();
    TesterMxLoopPtr tester=new TesterMxLoop(Org::DefaultWdt, Org::DefaultHgt);
    tester->Attach_Model(model);
    tester->Attach_StartingState(StartingState);
    //printf("Model Sanity1:\n"); tester->model->Print_Me();
    PopPtr pop = new Pop();
    pop->Assign_Params(100, tester, /* MaxOrgGens */ 10000, 2, /* EvoStagnationLimit */ 1500);
    //printf("Model Sanity2:\n"); ((TesterMxLoopPtr)(pop->tester))->model->Print_Me();
    pop->Evolve();
    delete pop;
    delete tester;
  }

int main() {
  cout << "Hello world!" << endl;
  time_t timer = time(NULL);
  //timer = 1508332512;
  //timer = 1509844801; // all zeros with onebit
 // timer = 1509902811;// all zeros
 // randseed:1510056985// with 2 models, goes to score 0.5000 for some reason
//Hello world!
//Mag:2.129535

  //timer = 1510141712;// 0.930000 final score
  //timer = 1510148948;// model testing
  //timer = 1510415166;
  //timer = 1510422187;// paradiddle GenCnt:1637, TopScore0:     0.69716139996346738, TopScore1:     0.57470560704264317
  //timer = 1510497058; // 5 beat, beats hypercube. use for testing other topologies

  //timer = 1510579661;// challenging paradiddle
  //timer = 1510663529;// also difficult
  //timer = 1510685430;
  //timer = 1510756519;
  //timer = 1510757758;// hard for mxloop

  //timer = 1510768785;// mxloop gets with some work

  printf("randseed:%lld\n", (long long)timer);
  srand(timer);
  char name[256];
  {
    MatrixPtr model = nullptr;
    VectPtr StartingState = nullptr;
    Evo_Model(model, StartingState);
    Evo_Mx(model, StartingState);
    delete model;
    delete StartingState;
    return 0;
  }
  PopPtr pop = new Pop();
  pop->Evolve();
  delete pop;
  return 0;
}

/*
so master plan is:
every link reads its input, iterates, and places its output.
every node reads the output vectors from every link and adds them together.
Then every node takes info from its central vector and puts it back into every link's input vectors.

next step is to create the test.
first create an all-to-all network, or random sparse, or hypercube or -?
test is generate a repeating pattern and push this pattern into CrossRoads vectors of some nodes for maybe 10 cycles.
Then stop inputting the pattern, and measure the ongoing states of those node hub vectors.
compare the hub vector values with the ongoing rhythm of the pattern generator.
those orgs that imitate the pattern best are fitter.
what is stopping condition for test?  just go for 10 cycles more, or even start with 1 cycle of the pattern.

pattern generator could just be another matrix iterator.

alternate approach: put matrix iterator in node.  node iterates, then loads output in hub vector.
hub is distributed to out-links.
then all vectors in the links are passed forward and summed in the other nodes.
then the nodes run their iterations, etc.


*/

