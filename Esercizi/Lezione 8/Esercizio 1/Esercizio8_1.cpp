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

    int n_equil = 10000; // Number of equilibration steps 

    double sigma = 0.630;
    double mu = 0.827;
    //vector<double> sigmas = {0.5, 0.75, 1.0, 1.25, 1.5};
    //vector<double> mus = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25, 1.5};    
    double delta = 2.6; // Proposal width for the Metropolis algorithm

    //for (double sigma : sigmas) {
      //  for (double mu : mus) {    

            auto prob_func = [sigma, mu](double x) { return prob(x, sigma, mu); };

            Results sampling_results;
            Results equilibration_results;

            cout << "Running Metropolis algorithm for 1D case..." << endl;

            double r0_1d = 0.0; // Initial position for 1D case

            // Equilibration phase
            equilibration_results = s.metropolis_1d(rnd, r0_1d, n_equil, delta, prob_func);
            r0_1d = equilibration_results.trajectory.back(); // Update initial position after equilibration

            // Sampling phase
            sampling_results = s.metropolis_1d(rnd, r0_1d, M, delta, prob_func);

            // Energy calculation
            vector<double> energies;
            energies.reserve(M);
            for (double x : sampling_results.trajectory) {
                energies.push_back(energy(x, sigma, mu));
            } 

            // Data blocking analysis
            Results energy_results = s.data_blocking(energies, nullptr, nullptr);

            energy_results.accepted = sampling_results.accepted;

            cout << "Metropolis algorithm completed." << endl;

            cout << "Sigma = " << sigma << ", Mu = " << mu << ", Delta = " << delta << endl;

            cout << "Acceptance rate: " << static_cast<double>(sampling_results.accepted) / M << endl;

            cout << "Energy = " << energy_results.ave_prog.back() << " ± " << energy_results.err_prog.back() << endl;
            
        //}
    //}

    // Print energy results
    s.Print(energy_results.ave_prog, "energy_mean.dat");
    s.Print(energy_results.err_prog, "energy_error.dat");
    s.Print(sampling_results.trajectory, "trajectory.dat");

    rnd.SaveSeed();
    return 0;
}