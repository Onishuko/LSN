#include "Simulation.h"
#include "Results.h"
#include "functions.h"

#include <iostream>
#include <cmath>
#include <fstream>
#include <iomanip>

using namespace std;

//Contains run and data_blocking methods
#pragma region Data blocking
   
/// @brief Runs the simulation with the given random number generator and functions
/// @param rnd Random number generator
/// @param func Function to evaluate with the random number generator (optional)
/// @param estimator Function to estimate the mean (optional)
/// @param transform Function to transform the mean (optional)
/// @param error_transform Function to transform the error (optional)
Results Simulation::run(Random& rnd, const function<double(Random&)> func, function<double(double)> estimator, function<double(double)> transform, function<double(double, double)> error_transform) {

    Results r;

    double sum_prog = 0;
    double sum2_prog = 0;

    // Loop over blocks
    for(int i = 0; i < N; i++){

        double sum = 0;

        // Loop over throws in each block
        for(int j = 0; j < L; j++){
            double x = func ? func(rnd) : rnd.Rannyu(); // Evaluate the function with the random number generator
            sum += estimator ? estimator(x) : x; // Use the estimator if provided, otherwise use the raw value
        }

        double mean = sum/L;

        r.ave.push_back(mean); // Store the mean of the current block

        sum_prog += mean;
        sum2_prog += mean * mean;

        double mean_prog = sum_prog/(i+1); // Calculate the progressive mean

        double err;

        // Calculate the error of the progressive mean
        if(i == 0){
            err = 0;
        } else {
            double mean2_prog = sum2_prog/(i+1);
            err = sqrt((mean2_prog - mean_prog*mean_prog)/i);
        }

        // Transform the progressive estimate if provided
        double meanfinal =
            transform ? transform(mean_prog) : mean_prog;

        // Transform the error using the analytically derived propagation formula
        double errfinal =
            error_transform ? error_transform(mean_prog, err) : err;

        r.ave_prog.push_back(meanfinal);
        r.err_prog.push_back(errfinal);
    }

    return r;
}

/// @brief Performs data blocking analysis on the given data
/// @param data Vector of data points
/// @param estimator Function to estimate the mean
/// @param transform Function to transform the mean
/// @param error_transform Function to transform the error
/// @return Results struct containing the analysis results
Results Simulation::data_blocking(const vector<double>& data, function<double(double)> estimator, function<double(double)> transform, function<double(double, double)> error_transform) {

    Results r;

    double sum_prog = 0;
    double sum2_prog = 0;

    for(int i = 0; i < N; i++){ // Loop over blocks
        
        double sum = 0;

        for(int j = 0; j < L; j++){ // Loop over throws in each block
            double x = data[i*L + j]; // Get the data point from the input vector
            sum += estimator ? estimator(x) : x; // Evaluate the function with the random number generator
        }

        double mean = sum/L;

        r.ave.push_back(mean); // Store the mean of the current block

        sum_prog += mean;
        sum2_prog += mean * mean;

        double mean_prog = sum_prog/(i+1);

        double err;

        if(i == 0){
            err = 0;
        } else {
            double mean2_prog = sum2_prog/(i+1);
            err = sqrt((mean2_prog - mean_prog*mean_prog)/i);
        }

        double meanfinal = transform ? transform(mean_prog) : mean_prog; // Apply the transformation function if provided

        double errfinal =
            error_transform ? error_transform(mean_prog, err) : err;

        r.ave_prog.push_back(meanfinal); // Store the progressive mean
        r.err_prog.push_back(errfinal); // Store the error of the progressive mean

    }
    return r;
}

#pragma endregion

//Contains run_RW method
#pragma region Random walk

/// @brief Performs a random walk simulation
/// @param rnd Random number generator
/// @param n_steps Number of steps in the random walk
/// @param func Function to generate random steps
/// @param estimator Function to estimate the mean
/// @param transform Function to transform the mean
/// @param error_transform Function to transform the error
/// @return Results struct containing the simulation results
Results Simulation::run_RW(Random& rnd, const int n_steps, const function<vector<double>(Random&)> func, function<double(double)> estimator, function<double(double)> transform, function<double(double, double)> error_transform) {

    Results r;

    r.ave_rw.resize(N, vector<double>(n_steps, 0.0));
    r.ave_rw_prog.resize(N, vector<double>(n_steps, 0.0));
    r.err_rw_prog.resize(N, vector<double>(n_steps, 0.0));

    vector<double> sum_prog(n_steps, 0.0);
    vector<double> sum2_prog(n_steps, 0.0);

    for(int i = 0; i < N; i++){ // Loop over blocks
        
        vector<double> sum_RW(n_steps, 0.0); // Initialize a vector to store the sum of random walks for each walk

        for(int j = 0; j < L; j++){ // Loop over throws in each block

            vector<double> x = func ? func(rnd) : vector<double>(rnd.Rannyu()); // Evaluate the function with the random number generator
            
            for(int k = 0; k < n_steps; k++){ // Loop over random walks 
                sum_RW[k] += estimator ? estimator(x[k]) : x[k];
            }
        }

        for(int k = 0; k < n_steps; k++){ // Loop over random walks to calculate the mean for each walk
            r.ave_rw[i][k] = sum_RW[k] / L;
        }
    }

    for(int k = 0; k < n_steps; k++){ // Loop over random walks to calculate the progressive mean and error for each walk
        for(int i = 0; i < N; i++){ // Loop over blocks
            sum_prog[k] += r.ave_rw[i][k];
            sum2_prog[k] += r.ave_rw[i][k] * r.ave_rw[i][k];

            double mean_prog = sum_prog[k] / (i + 1);
            double err;

            if(i == 0){
                err = 0;
            } else {
                double mean2_prog = sum2_prog[k] / (i + 1);
                err = sqrt((mean2_prog - mean_prog * mean_prog) / i);
            }

            double meanfinal = transform ? transform(mean_prog) : mean_prog; // Apply the transformation function if provided

            double errfinal = error_transform ? error_transform(mean_prog, err) : err; // Apply the error transformation function if provided

            r.ave_rw_prog[i][k] = meanfinal; // Store the progressive mean for each walk
            r.err_rw_prog[i][k] = errfinal; // Store the error of the progressive mean for each walk
        }
    }
    return r;
}

#pragma endregion

//Contains metropolis and metropolis_1d methods
#pragma region Metropolis

/// @brief Performs the Metropolis algorithm simulation
/// @param rnd Random number generator
/// @param r0 Initial position
/// @param n_steps Number of steps
/// @param delta Proposal width
/// @param prob Probability function
/// @param transition Transition type
/// @return Results struct containing the simulation results
Results Simulation::metropolis(Random& rnd, vector<double> r0, int n_steps, double delta, function<double(const vector<double>&)> prob, string transition){

    Results r;
    r.accepted = 0;
    r.positions.resize(n_steps, vector<double>(r0.size(), 0.0));

    vector<double> current_position = r0;
    double current_prob = prob(current_position);

    for(int i = 0; i < n_steps; i++){
        vector<double> proposed_position = current_position;

        // Generate a proposed position based on the specified transition type
        if(transition == "uniform"){
            for(size_t j = 0; j < proposed_position.size(); j++){
                proposed_position[j] += rnd.Rannyu(-delta, delta);
            }
        } else if(transition == "gaussian"){
            for(size_t j = 0; j < proposed_position.size(); j++){
                proposed_position[j] += rnd.Gauss(0.0, delta);
            }
        } else {
            cerr << "Error: Unknown transition type." << endl;
            return r;
        }

        // Calculate the acceptance ratio and decide whether to accept the proposed position
        double proposed_prob = prob(proposed_position);
        double acceptance_ratio;

        if(current_prob == 0.0){
            acceptance_ratio = (proposed_prob > 0.0) ? 1.0 : 0.0; // Handle the case where the current probability is zero
        } else {
            acceptance_ratio = min(1.0, proposed_prob / current_prob); // Calculate the acceptance ratio
        }


        // Accept or reject the proposed position based on the acceptance ratio
        if(rnd.Rannyu() < acceptance_ratio){
            current_position = proposed_position;
            current_prob = proposed_prob;
            r.accepted++;
        }

        // Store the current position in the positions vector
        r.positions[i] = current_position;
    }

    return r;

} 

/// @brief Performs the Metropolis algorithm simulation in one dimension
/// @param rnd Random number generator
/// @param r0 Initial position
/// @param n_steps Number of steps
/// @param delta Proposal width
/// @param prob Probability function
/// @return Results struct containing the simulation results
Results Simulation::metropolis_1d(Random& rnd, double r0, int n_steps, double delta, function<double(double)> prob){

    Results r;
    r.accepted = 0;
    r.trajectory.resize(n_steps);
    double current_position = r0;
    double current_prob = prob(current_position);

    for(int i = 0; i < n_steps; i++){
        
        // Generate a proposed position based on a uniform distribution
        double proposed_position = current_position + rnd.Rannyu(-delta, delta);

        // Calculate the acceptance ratio and decide whether to accept the proposed position
        double proposed_prob = prob(proposed_position);
        double acceptance_ratio;

        if(current_prob == 0.0){
            acceptance_ratio = (proposed_prob > 0.0) ? 1.0 : 0.0; // Handle the case where the current probability is zero
        } else {
            acceptance_ratio = min(1.0, proposed_prob / current_prob); // Calculate the acceptance ratio
        }

        // Accept or reject the proposed position based on the acceptance ratio
        if(rnd.Rannyu() < acceptance_ratio){
            current_position = proposed_position;
            current_prob = proposed_prob;
            r.accepted++;
        }

        // Store the current position in the positions vector
        r.trajectory[i] = current_position;
    }

    return r;

}

#pragma endregion

//Contains calculate_energy method
#pragma region Variatonal Monte Carlo

/// @brief Calculates the energy of a system using the Metropolis algorithm
/// @param rnd Random number generator
/// @param r0 Initial position
/// @param sigma Width parameter of the wave function
/// @param mu Center parameter of the wave function
/// @param nequil Number of equilibration steps
/// @param M Number of sampling steps
/// @param delta Proposal width
/// @return Results struct containing the energy calculation results
Results Simulation::calculate_energy(Random& rnd, double r0, double sigma, double mu, int nequil, int M, double delta) {
    Results r;

    // Define the probability function based on the provided sigma and mu
    auto prob_func = [sigma, mu](double x) { return prob(x, sigma, mu); };

    // Perform equilibration using the Metropolis algorithm
    Results equilibration_results = metropolis_1d(rnd, r0, nequil, delta, prob_func);
    r0 = equilibration_results.trajectory.back(); // Update initial position after equilibration

    // Perform sampling using the Metropolis algorithm
    Results sampling_results = metropolis_1d(rnd, r0, M, delta, prob_func);

    // Calculate energies for each sampled position
    vector<double> energies;
    energies.reserve(M);
    for (double x : sampling_results.trajectory) {
        energies.push_back(energy(x, sigma, mu));
    }

    // Perform data blocking analysis on the calculated energies
    Results energy_results = data_blocking(energies);

    energy_results.accepted = sampling_results.accepted;

    return energy_results;
}

#pragma endregion

//Contains simulated_annealing method
#pragma region Simulated annealing

/// @brief Performs simulated annealing to optimize the parameters of a system
/// @param rnd Random number generator
/// @param p Parameters struct containing the initial parameters and settings for the simulated annealing
/// @return 
Results Simulation::simulated_annealing(Random& rnd, const Parameters& p) {
    
    Results r;

    double sigma = p.sigma0;
    double mu = p.mu0;
    double T = p.T0;
    
    vector<double> energy_history;
    vector<double> energy_error_history;
    vector<double> sigma_history;
    vector<double> mu_history;

    energy_history.reserve(p.n_steps);
    energy_error_history.reserve(p.n_steps);
    sigma_history.reserve(p.n_steps);
    mu_history.reserve(p.n_steps);

    Results energy_results = calculate_energy(rnd, p.r0, sigma, mu, p.n_equil, p.M, p.delta);

    double current_energy = energy_results.ave_prog.back(); // Get the energy of the current configuration
    double current_error = energy_results.err_prog.back(); // Get the error of the energy of the current configuration


    for(int step = 0; step < p.n_steps; step++){

        // Propose new values for sigma and mu by adding a random perturbation
        double new_sigma = sigma + rnd.Rannyu(-p.dsigma, p.dsigma);
        double new_mu = mu + rnd.Rannyu(-p.dmu, p.dmu);

        // Ensure that sigma remains positive
        if (new_sigma <= 0.0) {

            energy_history.push_back(current_energy);
            energy_error_history.push_back(current_error);
            sigma_history.push_back(sigma);
            mu_history.push_back(mu);

            T *= p.cooling; // Update the temperature according to the cooling schedule
            continue; // Skip to the next iteration if new_sigma is not positive
        }
        
        // Calculate the energy of the new configuration using the Metropolis algorithm
        Results energy_results = calculate_energy(rnd, p.r0, new_sigma, new_mu, p.n_equil, p.M, p.delta);

        
        double new_energy = energy_results.ave_prog.back(); // Get the energy of the current configuration
        double new_error = energy_results.err_prog.back(); // Get the error of the energy of the current configuration

       double dE = new_energy - current_energy;

        // Calculate the acceptance probability based on the energy difference and current temperature
        double acceptance_probability = min(1.0, exp(-dE / T));

        // Accept or reject the new configuration based on the acceptance probability
        if(rnd.Rannyu() < acceptance_probability){
            mu = new_mu;
            sigma = new_sigma;
            current_energy = new_energy;
            current_error = new_error;
        }

        energy_history.push_back(current_energy);
        energy_error_history.push_back(current_error);
        sigma_history.push_back(sigma);
        mu_history.push_back(mu);

        T *= p.cooling; // Update the temperature according to the cooling schedule
    
    }

    r.energy_history = energy_history;
    r.energy_error_history = energy_error_history;
    r.sigma_history = sigma_history;
    r.mu_history = mu_history;

    return r;
  
}

#pragma endregion

//Contains printing methods
#pragma region Output

/// @brief Prints the data to a file with the specified filename
/// @param data vector of data to be printed
/// @param filename name of the output file
void Simulation::Print(const vector<double>& data, const string& filename){
    ofstream file(filename);

    if (!file) {
        cout << "Error: file not found" << endl;
        return;
    }

    file << fixed << setprecision(8);

    for (const auto& value : data) {
        file << value << endl;
    }

    file.close();
}

/// @brief Prints the last row of a vector to a file with the specified filename
/// @param data vector of data to be printed
/// @param filename name of the output file
void Simulation::Print(const vector<vector<double>>& data, const string& filename){
    ofstream file(filename);

    vector<double> temp(data.size());

    for (size_t i = 0; i < data.back().size(); ++i) {
        temp[i] = data.back()[i]; 
    }

    if (!file) {
        cout << "Error: file not found" << endl;
        return;
    }

    file << fixed << setprecision(8);

    for (const auto& value : temp) {
        file << value << endl;
    }

    file.close();
}

/// @brief Prints all data to a file with the specified filename
/// @param data Vector of vectors containing the data to be printed
/// @param filename Name of the output file
void Simulation::PrintAll(const vector<vector<double>>& data, const string& filename){
    ofstream file(filename);

    if (!file) {
        cout << "Error: file not found" << endl;
        return;
    }

    file << fixed << setprecision(8);

    for (const auto& row : data) {
        for (const auto& value : row) {
            file << value << "\t ";
        }
        file << endl;
    }

    file.close();
}

/// @brief Prints the positions to a file with the specified filename
/// @param positions Vector of vectors containing the positions to be printed
/// @param filename Name of the output file
/// @param steps Number of steps to skip between printed positions
void Simulation::PrintPositions(const vector<vector<double>>& positions, const string& filename, int steps) {
    ofstream file(filename);

    if (!file) {
        cout << "Error: file not found" << endl;
        return;
    }

    file << "x\ty\tz\n"; // Header for the columns

    file << fixed << setprecision(8);

    for(size_t i = 0; i < positions.size(); i++){
        
        if(i % steps == 0){ // Print every 'steps' positions
            for (const auto& value : positions[i]) {
                file << value << "\t ";
            }
            file << "\n";
        }
    }

    file.close();
}

#pragma endregion