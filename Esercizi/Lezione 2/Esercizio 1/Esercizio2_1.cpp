#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <cmath>
#include "../../../Parallel Random Number Generator-20260727/random.h"
#include "../../Simulation.h"
#include "../../functions.h"
#include "../../Results.h"

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

    rnd.SetRandom(rnd);

    Simulation u(M, N);
    Simulation c(M, N);

    function<double(Random&)> cumfunc = [](Random& rnd) {
        double y = rnd.Rannyu();
        return 1.0 - sqrt(1.0 - y); // Cumulative distribution function for the given distribution
    };

    // Define the transformation functions for the uniform distributions
    function<double(double)> meanfuncuni = [](double mean) {
        return M_PI_2 * cos(M_PI_2 * mean);
    };

    // Define the transformation function for the cumulative distribution
    function<double(double)> meanfunccum = [](double mean) {
        return (M_PI_2 * cos(M_PI_2 * mean)) / (2.0 * (1.0 - mean));
    };

    Results ru = u.run(rnd, nullptr, meanfuncuni, nullptr);
    Results rc = c.run(rnd, cumfunc, meanfunccum, nullptr);

    u.Print(ru.ave_prog, "ave_unif.txt");
    c.Print(rc.ave_prog, "ave_cum.txt");

    u.Print(ru.err_prog, "err_unif.txt");
    c.Print(rc.err_prog, "err_cum.txt");

    

    rnd.SaveSeed();
    return 0;
}