#ifndef TSP_H
#define TSP_H

#include <vector>
#include <string>

#include "../Parallel Random Number Generator-20260727/random.h"
#include "Individual.h"
#include "Results.h"

using namespace std;

class TSP {

    private:
    vector<vector<double>> cities;

    public:
    
    TSP(const vector<vector<double>>& cities);

    //Genetic algorithm's methods

    double cost(const vector<int>& path) const;

    bool check_path(const vector<int>& path) const;

    vector<int> random_path(Random& rnd) const;

    vector<Individual> generate_population(Random& rnd, int M) const;

    void sort_population(vector<Individual>& population) const;

    int select(Random& rnd, int M, double p) const;

    void mutate(Individual& individual, Random& rnd, double p_pair, double p_shift, double p_block, double p_inversion) const;
    
    pair<Individual, Individual> crossover(const Individual& parent1, const Individual& parent2, Random& rnd) const;

    vector<Individual> generate_new_population(const vector<Individual>& population, Random& rnd, double p_selection, double p_crossover, double p_pair, double p_shift, double p_block, double p_inversion, int n_elite) const;

    double best_half_average(const vector<Individual>& population) const;

    void print(const string& filename, const vector<double>& best, const vector<double> best_prog, const vector<double>& best_half) const;

    void print_best_path( const string& filename, const Individual& best_individual) const;

    //Parallel tempering's methods

    Individual trial_move(const Individual& current, Random& rnd) const;

    MetropolisStepResult metropolis_step(const Individual& current, Random& rnd, double T) const;

    bool exchange(Replica& replica1, Replica& replica2, Random& rnd) const;
    
    Replica initialize_replica(Random& rnd, double T) const;

    // Note: `metropolis` and `run_replica` definitions were removed from the implementation because they had no external usages in the current codebase.
    
}; 

#endif