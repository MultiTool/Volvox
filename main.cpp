#include <iostream>

#include "Base.h"
#include "FunSurf.h"
#include "Node.h"
#include "Cluster.h"
//#include "Stack.h"
#include "Pop.h"

using namespace std;

/* ********************************************************************** */
const double uprecision = 1000000.0;
double FullTime(struct timeval tm0) {// returns time in seconds and fractions of seconds
  return tm0.tv_sec + ((double)tm0.tv_usec)/uprecision;
}
struct timeval tm0, tm1;
/* ********************************************************************** */
void TimeCodeSample() {
  gettimeofday(&tm0, NULL);
  // do stuff here
  gettimeofday(&tm1, NULL);

  double t0num = FullTime(tm0);
  double t1num = FullTime(tm1);
  double delta = t1num-t0num;
  printf("delta T:%lf\n", delta);
}
/* ********************************************************************** */
const uint32_t ndims = 2;
int main() {
  srand(time(NULL));
  if (false) {
    double alt;
    NumVec nv;

    FunSurfGrid fs(ndims, 8);
    //fs.Create_Dummy_Ramp();
    fs.Create_Sigmoid_Deriv_Surface();
    if (false) {
      //nv.push_back(0.5);nv.push_back(0.5);nv.push_back(0.5);nv.push_back(0.5);
      nv.push_back(0.0); nv.push_back(0.0); nv.push_back(0.0); nv.push_back(0.0);
      alt = fs.Eval(&nv);
    }
    double invec[ndims];
    printf("\n");
    for (double step=-1.0; step<=1.0; step+=0.03) {
      for (int cnt=0; cnt<ndims; cnt++) {
        invec[cnt] = step;
      }
      alt = fs.Eval(invec);
      printf("step:%lf, alt:%lf\n", step, alt);
    }
    printf("\n");
    fs.Print_Me();
    printf("\n");

    double base = 1.0;
    printf("floor(base):%lf\n", floor(base));
    printf("floor(base-DBL_EPSILON):%lf\n", floor(base-DBL_EPSILON));
    printf("Fudge:%lf\n", Fudge/Fudge);
    printf("0.0/0.0:%lf\n", 0.0/0.0);
    return 0;
  }

  int numgens = 4000;
  numgens = 1000;
  int finalfail = 0;
  uint32_t num0, num1;
  double in0, in1;
  cout << "Hello world!" << endl;

  //next();
  /*
find a way to print final scores without the bottom ones being wiped
maybe train with a different random truth table each generation.
at least Shuffle each truth table.
  */

  if (true) {
    uint32_t evogens = 4000;
    evogens = 16000;
    //evogens = 1000;
    //evogens = 3;

    Pop pop;
    gettimeofday(&tm0, NULL);
    for (uint32_t gencnt=0; gencnt<evogens; gencnt++) {
      printf("\n");
      printf("-----------------------\n");
      printf("gencnt:%li, ", gencnt);
      pop.Gen(evogens, gencnt);
      // pop.Print_Sorted_Scores();
      gettimeofday(&tm1, NULL);
      double t0num = FullTime(tm0);
      double t1num = FullTime(tm1);
      double SecondsPassed = t1num-t0num;
      double MinutesPassed = SecondsPassed/60.0;
      double HoursPassed = MinutesPassed/60.0;
      printf(" HoursPassed:%lf, MinutesPassed:%lf, SecondsPassed:%lf\n", HoursPassed, MinutesPassed, SecondsPassed);
      //printf(" HoursPassed:%lf, SecondsPassed:%lf\n", HoursPassed, SecondsPassed);
    }
    //pop.ClayNet->Print_Me();
    pop.Print_Sorted_Scores();
    return 0;
  }
  return 0;
}

/*
Pop.Gen()
Space (NumCells=16):

   0.01117,    0.98542,    0.58671,    0.10058,
   0.35379,    0.70780,   -0.01187,    0.10801,
  -0.05393,    0.03004,   -0.77475,   -0.22737,
  -0.11991,   -0.59679,   -0.65068,   -0.10050,
bestbeast->Score:1.000000, 0.500500
 Score:1.000000, 0.500500
 Score:1.000000, 0.500500
 Score:1.000000, 0.500000
 Score:1.000000, 0.500000
 Score:1.000000, 0.250500
 Score:0.999500, 0.750000

 gencnt:3999, Pop.Gen()
Space (NumCells=16):

   0.14489,    0.37989,    0.55461,    0.04966,
   0.16378,    0.69230,    0.19582,    0.08316,
  -0.02024,   -0.26089,   -0.62332,   -0.52146,
  -0.10628,   -0.49616,   -0.48422,   -0.07547,
bestbeast->Score:1.000000, 0.500500
 Score:1.000000, 0.500500
 Score:1.000000, 0.500500
 Score:1.000000, 0.500000
 Score:1.000000, 0.250000
 Score:0.999500, 0.749500


this with a depth of 10 layers, not training the last 5:
gencnt:3999, Pop.Gen()
Space (NumCells=16):

   0.14593,    0.05171,    0.03419,    0.08996,
  -0.01653,    0.02561,    0.17296,    0.29763,
  -0.29288,   -0.15211,   -0.04438,    0.00766,
  -0.02522,   -0.06223,   -0.05057,   -0.00976,
bestbeast->Score:0.990000, 0.494500
 Score:0.990000, 0.494500
 Score:0.989500, 0.746000
 Score:0.989000, 0.498000
 Score:0.988500, 0.494500
 Score:0.988000, 0.743500
...
 Score:0.783000, 0.618500 at halfway

this with a depth of 10 layers, not training the last 5, but learning AND:
gencnt:1428, Pop.Gen()
Space (NumCells=16):

   0.08946,   -0.04429,   -0.05459,    0.11731,
   0.03244,    0.02438,   -0.01497,   -0.09420,
   0.02056,    0.03588,   -0.05977,   -0.10679,
   0.18926,    0.11900,   -0.08518,    0.02254,
bestbeast->Score:0.999000, 0.250000
gencnt:1429, Pop.Gen()


this with a depth of 10 layers, not training the last 5:
gencnt:3999, Pop.Gen()
Space (NumCells=16):

   0.29409,   -0.17846,   -0.41933,    0.07364,
   0.05719,   -0.09254,    0.00940,   -0.06120,
   0.23590,   -0.33456,   -0.08172,   -0.16637,
  -0.12547,    0.07739,    0.04874,   -0.28245,
bestbeast->Score:0.982000, 0.495500
leastbeast->Score:0.000500, -0.254500
 Score:0.982000, 0.495500
 Score:0.956500, 0.480000
 Score:0.954000, 0.478000
 Score:0.939500, 0.475500

*/
