#include "TSP.h"
#include <cmath>
#include <algorithm>
#include <fstream>
#include <iostream>

using namespace std;

TSP::TSP(const vector<vector<double>>& cities) : cities(cities) {}

//Contains generate_population, sort_population, select, crossover, generate_new_population and best_half_average methods
#pragma region Genetic algorithm

/// @brief Generates an initial population of individuals for the genetic algorithm
/// @param rnd The random number generator
/// @param M The size of the population
/// @return A vector of individuals representing the initial population
vector<Individual> TSP::generate_population(Random& rnd, int M) const{

    vector<Individual> population;
    population.reserve(M);

    for(int i = 0; i < M; i++){

        Individual individual;

        individual.path = random_path(rnd);
        individual.cost = cost(individual.path);

        population.push_back(individual);
    }

    return population;
}

/// @brief Sorts the population by increasing path cost
/// @param population The vector of individuals to sort
void TSP::sort_population(vector<Individual>& population) const{

    // Sort the population by increasing path cost,
    // so that the best individual is at the beginning.
    // sort is an'algorithm's function wich has 3 parameters:
    // 1) where the sorting begins
    // 2) where the sorting ends
    // 3) a lambda function that tells how the sorting works
    sort(population.begin(), population.end(), 
        [](const Individual& a, const Individual& b){
            return a.cost < b.cost;
        }
    );
}

/// @brief Selects an individual from the population based on a probability distribution
/// @param rnd The random number generator
/// @param M The size of the population
/// @param p The selection pressure parameter
/// @return The index of the selected individual
int TSP::select(Random& rnd, int M, double p) const{

    double r = rnd.Rannyu();

    int j = static_cast<int>(M * pow(r, p));

    return j;
}

/// @brief Performs crossover between two parent individuals to generate two offspring
/// @param parent1 The first parent individual
/// @param parent2 The second parent individual
/// @param rnd The random number generator
/// @return A pair of offspring individuals
pair<Individual, Individual> TSP::crossover(const Individual& parent1, const Individual& parent2, Random& rnd) const {

    int N = static_cast<int>(parent1.path.size());

    Individual child1, child2;
    child1.path.resize(N);
    child2.path.resize(N);

    child1.path[0] = 0;
    child2.path[0] = 0;

    //Choose the two crossover points
    int start = static_cast<int>(rnd.Rannyu(1, N - 1));
    int end = static_cast<int>(rnd.Rannyu(start + 1, N));

    //Keep track of the cities already inserted into each child
    vector<bool> used1(N, false);
    vector<bool> used2(N, false);
    used1[0] = true;
    used2[0] = true;

    //Copy segment from parent1 to child1 and segment from parent2 to child2
    for(int i = start; i < end; i++){

        child1.path[i] = parent1.path[i];
        used1[child1.path[i]] = true;

        child2.path[i] = parent2.path[i];
        used2[child2.path[i]] = true;
    }

    //Fill the remaining positions in child1 with parent2 cities
    int current1 = end;

    for(int i = 1; i < N; i++){

        int city = parent2.path[i];

        //Skip cities already used
        if(used1[city]) continue;

        //If the cycle reaches the end of the paths restart from the 2-nd position
        if(current1 == N) current1 = 1;

        child1.path[current1] = city;
        used1[city] = true;

        current1++;
    }

    //Fill the remaining positions in child2 with parent1 cities
    int current2 = end;

    for(int i = 1; i < N; i++){

        int city = parent1.path[i];

        //Skip cities already used
        if(used2[city]) continue;

        //If the cycle reaches the end of the paths restart from the 2-nd position
        if(current2 == N) current2 = 1;

        child2.path[current2] = city;
        used2[city] = true;

        current2++;
    }

    //Update children costs
    child1.cost = cost(child1.path);
    child2.cost = cost(child2.path);

    return {child1, child2};
}

/// @brief Generates a new population for the genetic algorithm
/// @param population The current population
/// @param rnd The random number generator
/// @param p_selection The selection pressure parameter
/// @param p_crossover The crossover probability
/// @param p_pair The probability of applying the pair mutation
/// @param p_shift The probability of applying the shift mutation
/// @param p_block The probability of applying the block mutation
/// @param p_inversion The probability of applying the inversion mutation
/// @param n_elite The number of elite individuals to preserve
/// @return A vector of individuals representing the new population
vector<Individual> TSP::generate_new_population(const vector<Individual>& population, Random& rnd, double p_selection, double p_crossover, double p_pair, double p_shift, double p_block, double p_inversion, int n_elite) const {

    int M = static_cast<int>(population.size());

    vector<Individual> new_population;
    new_population.reserve(M);

    //Elitism: preserve the best individual
    for(int i = 0; i < n_elite; i++) new_population.push_back(population[i]);

    for(int i = n_elite; i < M; i += 2){

        //Select two parents among the population
        int i1 = select(rnd, M, p_selection);
        int i2 = select(rnd, M, p_selection);

        //Generate two children
        Individual child1, child2;

        if (rnd.Rannyu() < p_crossover){
            auto [c1, c2] = crossover(population[i1], population[i2], rnd);
            child1 = c1;
            child2 = c2;
        } else {
            child1 = population[i1];
            child2 = population[i2];
        }
        
        //Apply mutations to the children
        mutate(child1, rnd, p_pair, p_shift, p_block, p_inversion);
        mutate(child2, rnd, p_pair, p_shift, p_block, p_inversion);

        //Add the first child to the new population
        new_population.push_back(child1);
        
        //Add the second child if there is still space
        if(i + 1 < M) new_population.push_back(child2);
    }

    return new_population;
}

/// @brief Calculates the average cost of the best half of the population
/// @param population The population of individuals
/// @return The average cost of the best half
double TSP::best_half_average(const vector<Individual>& population) const{

    int half = static_cast<int>(population.size() / 2);

    double sum = 0.0;

    for(int i = 0; i < half; i++) sum += population[i].cost;

    return sum / half;
}

#pragma endregion

//Contains trial_move, metropolis_step, metropolis, exchange, initialize_replica and replica methods
#pragma region Parallel tempering

/// @brief Generates a trial move for the parallel tempering algorithm
/// @param current The current individual
/// @param rnd The random number generator
/// @return The trial individual
Individual TSP::trial_move(const Individual& current, Random& rnd) const{

    //The copy is necessary because the metropolis algorithm may reject the proposed move
    Individual trial = current;

    //Choose one of the four mutation operators with equal probability
    int move = static_cast<int>(rnd.Rannyu(0.0, 4.0));

    switch(move){
        
        case 0:
        mutate(trial, rnd, 1.0, 0.0, 0.0, 0.0);
        break;

        case 1:
        mutate(trial, rnd, 0.0, 1.0, 0.0, 0.0);
        break;

        case 2:
        mutate(trial, rnd, 0.0, 0.0, 1.0, 0.0);
        break;

        case 3:
        mutate(trial, rnd, 0.0, 0.0, 0.0, 1.0);
        break;

        default:
        cerr << "ERROR: invalid move = " << move << endl;
        return current;
    }

    return trial;
}

/// @brief Performs a Metropolis step for the parallel tempering algorithm
/// @param current The current individual
/// @param rnd The random number generator
/// @param T The temperature
/// @return The result of the Metropolis step
MetropolisStepResult TSP::metropolis_step(const Individual& current, Random& rnd, double T) const{

    MetropolisStepResult result;

    // Check if the temperature is positive
    if(T <= 0.0){
        cerr << "ERROR: temperature must be positive" << endl;
        
        result.individual = current;
        result.accepted = false;

        return result;
    }

    // Generate a trial move and calculate the cost difference
    Individual trial = trial_move(current, rnd);

    double dL = trial.cost - current.cost;

    // Calculate the acceptance probability and decide whether to accept the trial move
    double acceptance_probability = min(1.0, exp(-dL / T));

    if(rnd.Rannyu() < acceptance_probability){

        result.individual = trial;
        result.accepted = true;
    } else {

        result.individual = current;
        result.accepted = false;
    }

    return result;

}

/// @brief Exchanges the states of two replicas in the parallel tempering algorithm
/// @param replica1 The first replica
/// @param replica2 The second replica
/// @param rnd The random number generator
/// @return True if the exchange was accepted, false otherwise
bool TSP::exchange(Replica& replica1, Replica& replica2, Random& rnd) const{

    double T1 = replica1.T;
    double T2 = replica2.T;

    double E1 = replica1.path.cost;
    double E2 = replica2.path.cost;

    double exponent = (1.0 / T1 - 1.0 / T2) * (E1 - E2);

    // Calculate the acceptance probability for the exchange
    double acceptance_probability = min(1.0, exp(exponent));

    // Decide whether to accept the exchange based on the acceptance probability
    if(rnd.Rannyu() < acceptance_probability){

        swap(replica1.path, replica2.path);

        return true;
    }

    return false;
}

/// @brief Initializes a replica for the parallel tempering algorithm
/// @param rnd The random number generator
/// @param T The temperature
/// @return The initialized replica
Replica TSP::initialize_replica(Random& rnd, double T) const{

    Replica replica;

    replica.T = T;

    replica.path.path = random_path(rnd);
    replica.path.cost = cost(replica.path.path);

    return replica;
}


#pragma endregion

//Contains cost, check_path, random_path and mutate methods
#pragma region Common functions

/// @brief Calculates the cost of a given path
/// @param path The path for which to calculate the cost
/// @return The cost of the path
double TSP::cost(const vector<int>& path) const{

    double L = 0.0;

    for(size_t i = 0; i < path.size(); i++){

        int current = path[i];
        int next = path[(i + 1) % path.size()]; //ritorna all'inizio quando arriva all'ultimo elemento del path
        
        double dx = cities[current][0] - cities[next][0];
        double dy = cities[current][1] - cities[next][1];

        L += sqrt(dx * dx + dy * dy);
    }

    return L;
}

/// @brief Checks if a given path is valid
/// @param path The path to check
/// @return True if the path is valid, false otherwise
bool TSP::check_path(const vector<int>& path) const{

    //Check if the path is empty
    if(path.empty()) return false;

    //Path must contain all the cities
    if(path.size() != cities.size()) return false;

    //First city must be the 0-th city
    if(path[0] != 0) return false;

    //Every city must appear only once
    vector<bool> visited(cities.size(), false);

    for(int city : path){

        //City must have a valid index
        if(city < 0 || city >= static_cast<int>(cities.size())) return false;

        //Every city must appear only once
        if(visited[city]) return false;

        //Mark the city as visited
        visited[city] = true;
    }

    return true;
}

/// @brief Generates a random path for the TSP
/// @param rnd The random number generator
/// @return A random path
vector<int> TSP::random_path(Random& rnd) const{

    vector<int> path;
    path.reserve(cities.size());

    //First city is always 0
    path.push_back(0);

    //Add all remaining cities
    for(int i = 1; i < static_cast<int>(cities.size()); i++) path.push_back(i);

    // Fisher-Yates shuffle of cities 1, ..., N-1.
    // At each step, city i is swapped with a randomly chosen
    // city among positions 1, ..., i. This generates a valid
    // random permutation while keeping city 0 fixed.
    for(int i = path.size() - 1; i > 1; i--){

        int j = static_cast<int>(rnd.Rannyu(1, i + 1));

        swap(path[i], path[j]);
    }

    return path;
}

/// @brief Mutates an individual by applying a random mutation operator
/// @param individual The individual to mutate
/// @param rnd The random number generator
/// @param p_pair The probability of applying the pair permutation operator
/// @param p_shift The probability of applying the shift operator
/// @param p_block The probability of applying the block permutation operator
/// @param p_inversion The probability of applying the inversion operator
void TSP::mutate(Individual& individual, Random& rnd, double p_pair, double p_shift, double p_block, double p_inversion) const{

    vector<int> path = individual.path;

    //Pair permutation
    if(rnd.Rannyu() < p_pair){
        
        int i = static_cast<int>(rnd.Rannyu(1, path.size()));

        int j = static_cast<int>(rnd.Rannyu(1, path.size()));

        while(j == i) j = static_cast<int>(rnd.Rannyu(1, path.size()));

        swap(path[i], path[j]);
    }

    //Shift 
    if(rnd.Rannyu() < p_shift){

        int N = static_cast<int>(path.size());

        //Choose the length of thee contiguous block to shift
        int m = static_cast<int>(rnd.Rannyu(1, N - 1));

        //Choose the starting position of the block
        int start = static_cast<int>(rnd.Rannyu(1, N - m));

        //Choose how main position the block is shifte to the right
        int shift = static_cast<int>(rnd.Rannyu(1, N - start -m + 1));

        // Shift a contiguous block of m cities to the right by 'shift' positions.
        // std::rotate moves the elements after 'middle' before the selected block,
        // preserving the order of all cities and keeping the path valid.
        rotate(path.begin() + start, path.begin() + start + m, path.begin() + start + m + shift);
    }

    //Block permutation
    if(rnd.Rannyu() < p_block){

        int N = static_cast<int>(path.size()); 

        //Choose the size of the block
        int m = static_cast<int>(rnd.Rannyu(1, N / 2));

        //Choose the starting position of the first block
        int s1 = static_cast<int>(rnd.Rannyu(1, N - 2 * m + 1));

        //Choose the starting position of the second block such as it is after the first one
        int s2 = static_cast<int>(rnd.Rannyu(s1 + m , N - m + 1));

        //Swap the two block while preserving the order of the cities inside each block
        for(int k = 0; k < m; k++) swap(path[s1 + k], path[s2 + k]);
    }

    //Inversion
    if(rnd.Rannyu() < p_inversion){

        int N = static_cast<int>(path.size());

        //Choose the first position of the block
        int start = static_cast<int>(rnd.Rannyu(1, N - 1));

        //Choose the last position of the block
        int end = static_cast<int>(rnd.Rannyu(start + 1, N));

        //Reverse invert every element in the range [start, end)
        reverse(path.begin() + start, path.begin() + end + 1);
    }

    //Update Individual
    individual.path = path;
    individual.cost = cost(path);
}

#pragma endregion

#pragma region Output

/// @brief Prints the best costs to a file
/// @param filename Name of the file to write to
/// @param best Vector of best costs
/// @param best_prog Vector of progressive best costs
/// @param best_half Vector of best costs for the best half of the population
void TSP::print(const string& filename, const vector<double>& best, const vector<double> best_prog, const vector<double>& best_half) const {

    ofstream file(filename);

    if(!file.is_open()){
        cerr << "Error opening file " << filename << endl;
        return;
    }

    for(size_t i = 0; i < best.size(); i++)  file << i << " " << best[i] << " " << best_prog[i] << " " << best_half[i] << " " << endl;

    file.close();
}

/// @brief Prints the best path to a file
/// @param filename Name of the file to write to
/// @param best_individual The best individual in the population
void TSP::print_best_path(const string& filename, const Individual& best_individual) const {

    ofstream file(filename);

    if(!file.is_open()){
        cerr << "Error opening file " << filename << endl;
        return;
    }

    for(int city : best_individual.path) file << cities[city][0] << " " << cities[city][1] << endl;

    // Add the first city again to close the path
    int first_city = best_individual.path[0];

    file << cities[first_city][0] << " " << cities[first_city][1] << endl;

    file.close();
}

#pragma endregion