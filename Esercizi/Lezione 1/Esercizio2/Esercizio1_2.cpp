#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <functional>
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

   int M = stoi(argv[1]);
   int N = stoi(argv[2]);

   if(M <= 0 || N <= 0 || M % N != 0){
      cerr << "Error: M must be positive, N must be positive, and M must be divisible by N." << endl;
      return 1;
   }

   // Set the random number generator
   rnd.SetRandom(rnd);

   Simulation sim(M, N);

   // Define the functions to be used in the simulation
   vector<function<double(Random&)>> functions = {
       [](Random& rnd) { return rnd.Rannyu(1, 6); },
       [](Random& rnd) { return rnd.Exp(1.0); },
       [](Random& rnd) { return rnd.Cauchy(1.0, 0.0);
       }
   };

   // Define the names of the functions for output file naming
   vector<string> function_names = {
       "Uniform Distribution",
       "Exponential Distribution",
       "Cauchy Distribution"
   };

   Results results;

   int L = static_cast<int>(M) / N;

   // Run the simulation for each function and print the results
   for(size_t i = 0; i < functions.size(); ++i){
       results = sim.run(rnd, functions[i]);
       sim.Print(results.ave, "ave_prog_" + function_names[i] + "_" + to_string(L) + ".txt");
   }

   rnd.SaveSeed();
   return 0;
}
