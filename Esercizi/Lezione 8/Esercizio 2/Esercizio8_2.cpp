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

// Inizio con una simulazione più breve per effettuare un fine tuning dei miei parametri iniziali poi ne eseguo una seconda molto più lunga partendo da condizioni iniziali più ragionevoli individuate dalla prima simulazione

int main(int argc, char* argv[]) {
    
    Random rnd;

    Parameters p;

    try
    {
        p = parse_arguments(argc, argv);
    }
    catch(const exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    // Print parameters

    cout << "Simulation parameters:" << endl;

    cout << "M       = " << p.M << endl;
    cout << "N       = " << p.N << endl;

    cout << "sigma0  = " << p.sigma0 << endl;
    cout << "mu0     = " << p.mu0 << endl;

    cout << "T0      = " << p.T0 << endl;
    cout << "dsigma  = " << p.dsigma << endl;
    cout << "dmu     = " << p.dmu << endl;

    cout << "n_steps = " << p.n_steps << endl;
    cout << "nequil  = " << p.n_equil << endl;

    cout << "cooling = " << p.cooling << endl;
    cout << "delta   = " << p.delta << endl;

    rnd.SetRandom(rnd);

    Simulation s(p.M, p.N);

    Results sa = s.simulated_annealing(rnd, p);

    s.Print(sa.energy_history, "sa_energy.dat");
    s.Print(sa.energy_error_history, "sa_error.dat");
    s.Print(sa.sigma_history, "sa_sigma.dat");
    s.Print(sa.mu_history, "sa_mu.dat");

    auto it = min_element(sa.energy_history.begin(), sa.energy_history.end());
    size_t i_min = distance(sa.energy_history.begin(), it);

    cout << "Simulated Annealing result:" << endl;
    cout << "Best SA step = " << i_min << endl;
    cout << "Best sigma = " << sa.sigma_history[i_min] << endl;
    cout << "Best mu = " << sa.mu_history[i_min] << endl;
    cout << "Energy = " << sa.energy_history[i_min] << " +/- " << sa.energy_error_history[i_min] << endl;
    
    rnd.SaveSeed();
    return 0;
}

/* Risultati prima simulazione:

Parametri iniziali:

int M = 10000;
int N = 100;
int n_equil = 1000; 
double r0 = 0.0; 
double sigma0 = 1.0 ;
double mu0 = 1.0;
double dsigma = 0.1;
double dmu = 0.1;
double T0 = 1.0;
double cooling = 0.99;
int n_steps = 500;
double delta = 3.5;

Parametri migliori:

Best SA step = 407
Best sigma = 0.611507
Best mu = 0.826712
Energy = -0.44749 +/- 0.00249917

Risultati seconda simulazione:

Simulation parameters
M       = 100000
N       = 100
sigma0  = 0.6
mu0     = 0.8
T0      = 1
dsigma  = 0.1
dmu     = 0.1
n_steps = 1000
nequil  = 10000
cooling = 0.995
delta   = 3.5

Simulated Annealing result:
Best SA step = 221
Best sigma = 0.605349
Best mu = 0.793831
Energy = -0.447541 +/- 0.00300248
*/