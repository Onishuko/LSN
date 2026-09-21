#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "../../../Parallel Random Number Generator-20260727/random.h"
#include "../../Simulation.h"
#include "../../Results.h"
#include "../../functions.h"

using namespace std;

int main(int argc, char* argv[]) {
   Random rnd;
   
   if(argc != 3){
      cerr << "Usage: " << argv[0] << " <M> number of throws <N> number of blocks" << endl;
      return 1;
   }

   //Initialize number of throws and number of blocks
   int M = stoi(argv[1]);
   int N = stoi(argv[2]);

   if(M <= 0 || N <= 0 || M % N != 0){
      cerr << "Error: M must be positive, N must be positive, and M must be divisible by N." << endl;
      return 1;
   }

   //Initialize random number generator
   rnd.SetRandom(rnd);

   //Initialize simulation with datablocking parameters (M, L and L=M/N)
   Simulation sim(M, N);

   //Saves data in an specialized struct
   Results results = sim.run(rnd, [](Random& rnd) { return rnd.Rannyu(); });

   //Saves error data
   Results results_err = sim.run(rnd, [](Random& rnd) { 
      double x = rnd.Rannyu() -0.5;
      return x * x;
   });

   //Print resutls in a file.dat
   sim.Print(results.ave_prog, "ave_prog.txt");
   sim.Print(results.err_prog, "err_prog.txt");
   sim.Print(results_err.ave_prog, "ave_prog_err.txt");
   sim.Print(results_err.err_prog, "err_prog_err.txt");

   //Compute and print chi squared values
   vector<double> chi2_values = chi2(100, 10000, 100, rnd);
   sim.Print(chi2_values, "chi2.txt");

   rnd.SaveSeed();
   return 0;
}
