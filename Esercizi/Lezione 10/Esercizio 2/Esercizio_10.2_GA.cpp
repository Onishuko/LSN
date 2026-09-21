#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <fstream>
#include <limits>

#include "../../TSP.h"
#include "../../../Parallel Random Number Generator-20260727/random.h"
#include "../../Cities.h"

using namespace std;


int main() {

    const int N = 110; //Number of cities
    const int M = 10; //Population size
    const int n_generations = 70000; 
    const int n_elite = 1; //Number of elite individuals

    const double p = 2.0; //Parameter
    const double p_crossover = 0.6;

    const double p_pair = 0.05;
    const double p_shift = 0.05;
    const double p_block = 0.05;
    const double p_inversion = 0.05;

    Random rnd;
    rnd.SetRandom(rnd);

    //Read cities from file
    vector<vector<double>> cities = read_cities("cap_prov_ita.dat", N);

    //Create TSP
    TSP tsp(cities);

    //Generate initial population
    vector<Individual> population = tsp.generate_population(rnd, M);
    tsp.sort_population(population);

    //Check initial population
    bool initial_valid = true;

    for(const Individual& individual : population){
        if(!tsp.check_path(individual.path)){
            initial_valid = false;
            break;
        }
    }

    cout << "Initial population valid: " << (initial_valid ? "YES" : "NO") << endl;

    //Initialize vector for best_cost and best_average_half
    vector<double> best;
    best.reserve(n_generations);

    vector<double> best_prog;
    best_prog.reserve(n_generations);

    vector<double> best_half;
    best_half.reserve(n_generations);

    //Takes de maximum value of double numbers
    double current_best = numeric_limits<double>::max();
    Individual best_individual = population[0];

    //Genetic algorithm
    for(int gen = 0; gen < n_generations; gen++){

        double best_cost = population[0].cost;
        double average_best = tsp.best_half_average(population);

        best.push_back(best_cost);

        if(best_cost < current_best){
            current_best = best_cost;
            best_prog.push_back(current_best);

            best_individual = population[0];
        } else {
            best_prog.push_back(current_best);
        }

        best_half.push_back(average_best);

        population = tsp.generate_new_population(population, rnd, p, p_crossover, p_pair, p_shift, p_block, p_inversion, n_elite);

        tsp.sort_population(population);
    }

    //Final population check
    bool final_valid = true;

    for(const Individual& individual : population){
        if(!tsp.check_path(individual.path)){
            final_valid = false;
            break;
        }
    }

    cout << "Final population valid: " << (final_valid ? "YES" : "NO") << endl;
    cout << "Best final cost: " << population[0].cost << endl;
    cout << "Best progressive cost: " << best_prog.back() << endl;

    //Print data

    //Print best cost and everage half
    tsp.print("Best_cost_GA.dat", best, best_prog, best_half);

    //Print best coordinate
    tsp.print_best_path("Best_path_GA.dat", best_individual);

    return 0;
}