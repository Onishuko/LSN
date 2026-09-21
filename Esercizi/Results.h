#ifndef __RESULTS__
#define __RESULTS__

#include<vector>
#include "Individual.h"

using namespace std;

/// @brief Struct to hold the results of the simulation
struct Results{
    
    // Results for the run() and data_blocking() methods
    vector<double> ave; //mean of the blocks
    vector<double> ave_prog; //progressive mean
    vector<double> err_prog; //error of the progressive mean
    
    // Results for the run_RW() method
    vector<vector<double>> ave_rw; //mean of the random walks
    vector<vector<double>> ave_rw_prog; //progressive mean of the random walks
    vector<vector<double>> err_rw_prog; //progressive mean of the squares of the random

    // Results for the metropolis() method
    vector<double> trajectory; //trajectory of the Metropolis algorithm in one dimension
    vector<vector<double>> positions; //positions of the Metropolis algorithm in multiple dimensions
    int accepted = 0; //number of accepted moves in the Metropolis algorithm

    // Results for the simulated_annealing() method
    vector<double> energy_history; //history of the energy during simulated annealing
    vector<double> energy_error_history; //history of the energy error during simulated annealing
    vector<double> sigma_history; //history of the sigma parameter during simulated annealing
    vector<double> mu_history; //history of the mu parameter during simulated annealing
    
};

/// @brief Struct to hold the results of a single Metropolis step
struct MetropolisStepResult{
    
    Individual individual; //individual representing the current state of the system
    bool accepted; //flag indicating whether the proposed move was accepted
};

/// @brief Struct to hold the results of the Metropolis algorithm
struct  MetropolisResults{
    
    Individual final; //individual representing the final state of the system
    Individual best; //individual representing the best state of the system found during the simulation

    vector<double> cost_history; //history of the cost function during the Metropolis algorithm
    vector<double> best_history; //history of the best cost function found during the Metropolis algorithm

    int accepted = 0; //number of accepted moves in the Metropolis algorithm
};

/// @brief Struct to hold the parameters for a replica in the parallel tempering algorithm
struct Replica{
    
    double T;
    Individual path;
};

/// @brief Struct to hold the parameters for the Metropolis algorithm
struct MetropolisCase{

    string state; //state of the system
    string transition; //transition type
    double delta; //proposal width
    function<double(const vector<double>&)> prob; //probability function
};

/// @brief Struct to hold the parameters for the Metropolis algorithm in one dimension
struct MetropolisCase1D{

    string state; //state of the system
    string transition; //transition type
    double delta; //proposal width
    function<double(double)> prob; //probability function
};

/// @brief Struct to hold the parameters for the simulated annealing algorithm
struct Parameters{

    int M;
    int N;

    double r0;
    double sigma0;
    double mu0;

    double T0;
    double dsigma;
    double dmu;

    int n_steps;
    int n_equil;

    double cooling;
    double delta;
};

#endif

