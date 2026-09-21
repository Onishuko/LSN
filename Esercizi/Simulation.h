#ifndef __SIMULATION__
#define __SIMULATION__

#include <functional>
#include <vector>
#include <string>

#include "../Parallel Random Number Generator-20260727/random.h"
#include "Results.h"

using namespace std;

class Simulation {

    private:
        
        int M; // Total number of throws
        int N; // Number of blocks
        int L; // Number of throws in each block

    public:
        /// @brief Constructor for the Simulation class
        /// @param m The total number of throws
        /// @param n The number of blocks
        Simulation(int m, int n) : M(m), N(n), L(m / n) {}

        Results run(Random& rnd, const function<double(Random&)> func = nullptr, function<double(double)> mean_func = nullptr, function<double(double)> transform = nullptr, function<double(double, double)> error_transform = nullptr);

        Results data_blocking(const vector<double>& data, function<double(double)> mean_func = nullptr, function<double(double)> transform = nullptr, function<double(double, double)> error_transform = nullptr);

        Results run_RW(Random& rnd, const int n_walks, const function<vector<double>(Random&)> func = nullptr, function<double(double)> mean_func = nullptr, function<double(double)> transform = nullptr, function<double(double, double)> error_transform = nullptr);

        Results metropolis(Random& rnd, vector<double> r0, int n_steps, double delta, function<double(const vector<double>&)> prob, string transition);

        Results metropolis_1d(Random& rnd, double r0, int n_steps, double delta, function<double(double)> prob);

        Results calculate_energy(Random& rnd, double r0, double sigma, double mu, int nequil, int M, double delta);

        Results simulated_annealing(Random& rnd, const Parameters& p);

        void Print(const vector<double>& data, const string& filename);

        void Print(const vector<vector<double>>& data, const string& filename);

        void PrintAll(const vector<vector<double>>& data, const string& filename);

        void PrintPositions(const vector<vector<double>>& positions, const string& filename, int steps);

};

#endif

