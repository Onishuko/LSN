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

    Simulation s(M, N);

    double S_0 = 100.0; // Initial stock price
    double K = 100.0;   // Strike price
    double T = 1.0;     // Time to maturity
    double r = 0.1;     // Risk-free interest rate
    double sigma = 0.25; // Volatility
    double dt = 0.01;   // Time step for discrete simulation

    // Define the function for the direct simulation of stock price at maturity
    function<double(Random&)> s_direct = [S_0, T, r, sigma](Random& rnd) {
        double W = rnd.Gauss(0.0, sqrt(T));
        double S_T = S_0 * exp((r - 0.5 * sigma * sigma) * T + sigma * W);
        return S_T;
    };

    // Define the function for the discrete simulation of stock price at maturity
    function<double(Random&)> s_disc = [S_0, T, r, sigma, dt](Random& rnd) {
        double S_T = S_0;
        for(int i = 0; i < T/dt; i++){
            double W = rnd.Gauss(0.0, sqrt(dt));
            S_T *= exp((r - 0.5 * sigma * sigma) * dt + sigma * W);
        }
        return S_T ;
    };

    // Define the payoff functions for call options
    function<double(double)> call = [K, r, T](double S_T) {
        return exp(-r * T) * max(0.0, S_T - K);
    };

    // Define the payoff functions for put options
    function<double(double)> put = [K, r, T](double S_T) {
        return exp(-r * T) * max(0.0, K - S_T);
    };

    vector<Results> results = {
        s.run(rnd, s_direct, call),
        s.run(rnd, s_direct, put),
        s.run(rnd, s_disc, call),
        s.run(rnd, s_disc, put)
    };

    for(size_t i = 0; i < results.size(); ++i){
        string type = (i % 2 == 0) ? "call" : "put";
        string method = (i < 2) ? "direct" : "discrete";
        s.Print(results[i].ave_prog, method + "_" + type + "_ave_prog.txt");
        s.Print(results[i].err_prog, method + "_" + type + "_err_prog.txt");
    }

    rnd.SaveSeed();
    return 0;
}