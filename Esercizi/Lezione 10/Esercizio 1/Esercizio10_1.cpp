#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <mpi.h>

#include "../../TSP.h"
#include "../../Individual.h"
#include "../../Results.h"
#include "../../Exchange.h"
#include "../../Cities.h"
#include "../../../Parallel Random Number Generator-20260727/random.h"


using namespace std;


// Initialize random number according to the MPI rank so that different processes use different random sequence

void SetRandom(Random& rnd, int rank){

    int seed[4];
    int p1, p2;

    ifstream Primes("../../../Parallel Random Number Generator-20260727/Primes");

    if(Primes.is_open()){
    Primes >> p1 >> p2;
    } else {
        cerr << "Unable to open Primes" << endl;
        return;
    }

    Primes.close();

    ifstream input("../../../Parallel Random Number Generator-20260727/seed.in");

    string property;

    if(input.is_open()){

        while(!input.eof()){

            input >> property;

            if(property == "RANDOMSEED"){
                input >> seed[0] >> seed[1] >> seed[2] >> seed[3];

                seed[0] += 100 * rank;
                seed[1] += 100 * rank;
                seed[2] += 100 * rank;
                seed[3] += 100 * rank;

                rnd.SetRandom(seed, p1, p2);
            }

        }
        input.close();
    } else {
        cerr << "Unable to open seed.in" << endl;
        return;
    }

}

int main(int argc, char* argv[]){

    //MPI initialization
    MPI_Init(&argc, &argv);

    int rank;
    int size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Parameters
    const int N = 34;
    const int n_steps = 100000;

    //Number of Metropolis steps between exchanges
    const int N_exch = 10;

    double T_min = 0.1;
    double T_max = 0.8;

    //Check numbers of processes
    if(size < 2 || size > 11){
        if(rank == 0){
            cerr << "Number of processes must be between 2 and 11" << endl;
        }
        MPI_Finalize();

        return 1;
    }

    //Random generator for cities (all ranks must generate all the cities therfore rank = 0 is used)
    Random city_rnd;
    SetRandom(city_rnd, 0);

    //Generate city
    function<vector<double>(Random&)> circle = [](Random& rnd){

        double theta = 2.0 * M_PI * rnd.Rannyu();

        vector<double> city_circle(2);

        city_circle[0] = cos(theta);
        city_circle[1] = sin(theta);

       return city_circle;

    };

    vector<vector<double>> cities = generate_cities(city_rnd, N, circle);

    //Initialze TSP
    TSP tsp(cities);

    //Random number generator for the algorithm
    Random rnd;
    SetRandom(rnd, rank);

    double T = T_min * pow(T_max / T_min, static_cast<double>(rank) / static_cast<double>(size - 1));

    //Initialize replica
    Replica replica = tsp.initialize_replica(rnd, T);

    Individual best = replica.path;

    cout << fixed << setprecision(6);
    cout << "INITIAL CONFIGURATION" << endl;
    cout << "Rank " << rank << endl;
    cout << "Initial temperature = " << replica.T << endl;
    cout << "Cost = " << replica.path.cost << endl;
    cout << "Valid: " << (tsp.check_path(replica.path.path) ? "YES" : "NO") << endl;
    cout << "Path: ";
    for(int city : replica.path.path) cout << city << " ";
    cout << endl;
    cout << endl;

    int attempted_exchange = 0;
    int accepted_exchange = 0;

    //Parallel tempering
    for(int step = 0; step < n_steps; step++){

        //Metropolis step
        MetropolisStepResult result = tsp.metropolis_step(replica.path, rnd, replica.T);

        //Update replica if move is accepted
        if(result.accepted) replica.path = result.individual;
        if(replica.path.cost < best.cost) best = replica.path;

        //Exchange between adjacent temperatures
        if((step + 1) % N_exch == 0){

            //We alternate between two alternating pattern. EVEN -> ODD and ODD -> EVEN
            int exchange_phase = (step / N_exch) % 2;

            //EVEN -> ODD
            if(exchange_phase == 0){
                if(rank % 2 == 0){
                    //Even rank communicates with rank +1
                    if(rank + 1 < size){
                         bool accepted = exchange_mpi(tsp, replica, rank + 1, rnd, MPI_COMM_WORLD);
                         attempted_exchange++;
                         if(accepted) accepted_exchange++;
                    }
                } else {
                    //Odd rank communicates with rank - 1
                    bool accepted = exchange_mpi(tsp, replica, rank - 1, rnd, MPI_COMM_WORLD);
                    attempted_exchange++;
                    if(accepted) accepted_exchange++;
                }
            } else { //ODD -> EVEN
                if(rank % 2 == 1){
                    //Odd rank communicates with rank +1
                    if(rank + 1 < size){
                         bool accepted = exchange_mpi(tsp, replica, rank + 1, rnd, MPI_COMM_WORLD);
                         attempted_exchange++;
                         if(accepted) accepted_exchange++;
                    }
                } else {
                    //Even rank communicate with rank - 1
                    if(rank - 1 >= 0){
                        bool accepted =  exchange_mpi(tsp, replica, rank - 1, rnd, MPI_COMM_WORLD);
                        attempted_exchange++;
                        if(accepted) accepted_exchange++;
                    }
                }
            }
        }
    }

    double acceptance_rate = 0.0;

    if(attempted_exchange > 0) acceptance_rate = static_cast<double>(accepted_exchange) / attempted_exchange;

    cout << "ACCEPTANCE" << endl;
    cout << "Rank " << rank << endl;
    cout << "Exchange attempted = " << attempted_exchange << endl;
    cout << "Exchange accepted = " << accepted_exchange << endl;
    cout << "Acceptance rate = " << acceptance_rate << endl;
    cout << endl;

    cout << "FINAL CONFIGURATION" << endl;
    cout << "Rank " << rank << endl;
    cout << "Final temperature = " << replica.T << endl;
    cout << "Cost = " << replica.path.cost << endl;
    cout << "Valid: " << (tsp.check_path(replica.path.path) ? "YES" : "NO") << endl;
    cout << "Path: ";
    for(int city : replica.path.path) cout << city << " ";
    cout << endl;
    cout << endl;

    double best_cost = best.cost;
    vector<double> all_best_cost;
    
    if(rank == 0) all_best_cost.resize(size);

    MPI_Gather(&best_cost, 1, MPI_DOUBLE, all_best_cost.data(), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    vector<int> all_best_paths;

    if(rank == 0) all_best_paths.resize(size * N);

    MPI_Gather(best.path.data(), N, MPI_INT, all_best_paths.data(), N, MPI_INT, 0, MPI_COMM_WORLD);

    if(rank == 0){
        int best_rank = 0;

        for(int r = 1; r < size; r++){
            if(all_best_cost[r] < all_best_cost[best_rank]) best_rank = r;
        }

        cout << endl;
        cout << "BEST SOLUTION" << endl;
        cout << "Rank = " << best_rank << endl;
        cout << "Temperature = " << T_min * pow(T_max / T_min, static_cast<double>(best_rank) / static_cast<double>(size - 1)) << endl;
        cout << "Cost = " << all_best_cost[best_rank] << endl;
        cout << "Path = ";
        for(int i = 0; i < N; i++) cout << all_best_paths[best_rank * N +i] << " ";
        cout << endl;
        cout << endl;
    }

    // MPI FINALIZATION
    MPI_Finalize();

    return 0;
}



// /usr/bin/mpicxx -std=c++17 -Wall Esercizio10_1.cpp ../../TSP.cpp ../../Cities.cpp ../../Exchange.cpp ../../../Parallel\ Random\ Number\ Generator-20260727/random.cpp -o main
// /usr/bin/mpirun --oversubscribe -np 10 ./main