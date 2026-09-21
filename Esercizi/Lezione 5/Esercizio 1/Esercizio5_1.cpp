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

    // Define a struct to hold the parameters for each Metropolis case
    vector<MetropolisCase> cases = {
        {"1s", "uniform", 1.2, [](const vector<double>& r) { return prob(r, "1s"); }},
        {"2p", "uniform", 3.0, [](const vector<double>& r) { return prob(r, "2p"); }},
        {"1s", "gaussian", 0.75, [](const vector<double>& r) { return prob(r, "1s"); }},
        {"2p", "gaussian", 1.85, [](const vector<double>& r) { return prob(r, "2p"); }}
    };

    vector<Results> results;
    vector<Results> equilibration_results;

    for (const auto& mc : cases) {
        cout << "Running Metropolis for state: " << mc.state << ", transition: " << mc.transition << endl;

        vector<double> x;
        if(mc.state == "1s") {
            x = {0.0, 0.0, 0.0}; // Initial position at the origin
        } else if (mc.state == "2p") {
            x = {0.0, 0.0, 1.0}; // Initial position along the z-axis
        } else {
            cerr << "Error: Unknown state." << endl;
            return 1;
        }

        // Equilibration phase
        Results equil = s.metropolis(rnd, x, n_equil, mc.delta, mc.prob, mc.transition);
        x = equil.positions.back(); // Use the last position after equilibration as the starting point

        // Calculate the progressive mean of the radii during equilibration
        vector<double> equil_prog;
        equil_prog.reserve(n_equil);

        double sum_equil = 0.0;

        for (int i = 0; i < n_equil; i++) {
            const auto& pos = equil.positions[i];
            double radius = sqrt(pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]);
            sum_equil += radius;
            equil_prog.push_back(sum_equil / (i + 1) );
        }

        Results equil_data;
        equil_data.ave_prog = equil_prog;
        equilibration_results.push_back(equil_data);

        // Run the Metropolis algorithm for M steps
        Results sampling = s.metropolis(rnd, x, M, mc.delta, mc.prob, mc.transition);

        string sampling_filename = "sampling_" + mc.state + "_" + mc.transition + ".dat";
        s.PrintPositions(sampling.positions, sampling_filename, 100); 

        
        // radii of the sampled positions
        vector<double> radii;
        radii.reserve(M);
        for (const auto& pos : sampling.positions) {
            radii.push_back(sqrt(pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]));
        }

        // Data blocking to compute the mean and error of the radii
        Results r = s.data_blocking(radii, nullptr, nullptr);

        r.accepted = sampling.accepted; // Store the number of accepted moves
        results.push_back(r);

        cout << "Acceptance rate (sampling): " << static_cast<double>(r.accepted) / M << endl;
    }

    for (size_t i = 0; i < results.size(); i++){
      string mean_filename = "mean_" + cases[i].state + "_" + cases[i].transition + ".dat";
      string error_filename = "error_" + cases[i].state + "_" + cases[i].transition + ".dat";
      s.Print(results[i].ave_prog, mean_filename);
      s.Print(results[i].err_prog, error_filename);
    }

    for(size_t i = 0; i < equilibration_results.size(); i++){
      string equil_filename = "equilibration_" + cases[i].state + "_" + cases[i].transition + ".dat";
      s.Print(equilibration_results[i].ave_prog, equil_filename);
    }

    rnd.SaveSeed();
    return 0;
}