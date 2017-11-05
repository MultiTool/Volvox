#include <iostream>

using namespace std;

//#include "Matrix.hpp"
//#include "Link.hpp"
#include "Node.hpp"
#include "Cluster.hpp"
#include "Pop.hpp"

int main() {
  cout << "Hello world!" << endl;
  time_t timer = time(NULL);
  //timer = 1508332512;
  //timer = 1509844801; // all zeros with onebit
  timer = 1509902811;// all zeros
//Hello world!
//Mag:2.129535

  printf("randseed:%lld\n", (long long)timer);
  srand(timer);
  char name[256];
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

