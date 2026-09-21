#include "functions.h"

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

/// @brief Calculates the chi-squared values for a given number of bins, throws, and trials
/// @param n Number of bins
/// @param m Number of throws
/// @param t Number of trials
/// @param rnd Random number generator
/// @return Vector of chi-squared values
vector<double> chi2(int n, int m, int t, Random& rnd) {
    vector<double> chi2_values;
    double expected = static_cast<double>(m) / n;

    for (int i = 0; i < t; ++i) { // Loop over trials
        vector<int> counts(n, 0);
        for (int j = 0; j < m; ++j) { // Loop over throws
            int bin = static_cast<int>(rnd.Rannyu() * n); // Determine which bin the random number falls into
            counts[bin]++; // Increment the count for that bin
        }

        double chi2_value = 0.0;
        for (int k = 0; k < n; ++k) { // Loop over bins to calculate chi-squared value
            chi2_value += pow(counts[k] - expected, 2) / expected;
        }
        chi2_values.push_back(chi2_value);
    }
    
    return chi2_values;
}

/// @brief Calculates the probability for a given position and orbital type
/// @param r Position vector
/// @param type Orbital type
/// @return Probability value
double prob(vector<double> r, string type){

    double rad = sqrt(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);

    if(type == "1s") return exp(-2.0 * rad);

    else if(type == "2p") return r[2] * r[2] * exp(-rad);

    else return 0.0;
}

/// @brief Calculates the wave function for a given position and parameters
/// @param x Position
/// @param sigma Width parameter
/// @param mu Center parameter
/// @return Wave function value
double psi(double x, double sigma, double mu) {
    
    return exp(-pow((x - mu), 2) / (2 * sigma * sigma)) + exp(-pow((x + mu), 2) / (2 * sigma * sigma));

}

/// @brief Calculates the second derivative of the wave function for a given position and parameters
/// @param x Position
/// @param sigma Width parameter
/// @param mu Center parameter
/// @return Second derivative of the wave function value
double ddpsi(double x, double sigma, double mu) {
    
    double a = mu / (sigma * sigma);

    double factor =
        -1.0 / (sigma * sigma)
        + a * a / (cosh(a * x) * cosh(a * x))
        + pow(-x / (sigma * sigma) + a * tanh(a * x), 2);

    return factor * psi(x, sigma, mu);
}

/// @brief Calculates the probability for a given position and parameters
/// @param x Position
/// @param sigma Width parameter
/// @param mu Center parameter
/// @return Probability value
double prob(double x, double sigma, double mu){

    double psi_val = psi(x, sigma, mu);

    return psi_val * psi_val;
}


/// @brief Calculates the energy for a given position and parameters
/// @param x Position
/// @param sigma Width parameter
/// @param mu Center parameter
/// @return Energy value
double energy(double x, double sigma, double mu){
    
    double psi_val = psi(x, sigma, mu);
    double ddpsi_val = ddpsi(x, sigma, mu);

    return -0.5 * ddpsi_val / psi_val + x * x * x * x - 2.5 * x * x; // Energy = -1/2 * (d^2 psi / dx^2) / psi + V(x) with V(x) = x^4 - 2.5 * x^2
}


/// @brief Parses command line arguments and returns a Parameters struct
Parameters parse_arguments(int argc, char* argv[]) {

    if (argc != 13) {
        cerr << "Usage: " << argv[0]
             << " <M> <N> <r0> <sigma0> <mu0> <T0> "
             << "<dsigma> <dmu> <n_steps> <nequil> "
             << "<cooling> <delta>"
             << endl;

        throw invalid_argument("Wrong number of arguments");
    }

    Parameters p;

    try {
        p.M       = stoi(argv[1]);
        p.N       = stoi(argv[2]);
        p.r0      = stod(argv[3]);
        p.sigma0  = stod(argv[4]);
        p.mu0     = stod(argv[5]);
        p.T0      = stod(argv[6]);
        p.dsigma  = stod(argv[7]);
        p.dmu     = stod(argv[8]);
        p.n_steps = stoi(argv[9]);
        p.n_equil  = stoi(argv[10]);
        p.cooling = stod(argv[11]);
        p.delta   = stod(argv[12]);
    }
    catch (const invalid_argument& e) {
        cerr << "Error: invalid numerical argument." << endl;
        throw;
    }
    catch (const out_of_range& e) {
        cerr << "Error: numerical argument out of range." << endl;
        throw;
    }

    // Checks on the parameters

    if (p.M <= 0) {
        cerr << "Error: M must be positive." << endl;
        throw invalid_argument("Invalid M");
    }

    if (p.N <= 0) {
        cerr << "Error: N must be positive." << endl;
        throw invalid_argument("Invalid N");
    }

    if (p.M % p.N != 0) {
        cerr << "Error: M must be divisible by N." << endl;
        throw invalid_argument("Invalid M / N");
    }

    if (p.sigma0 <= 0.0) {
        cerr << "Error: sigma0 must be positive." << endl;
        throw invalid_argument("Invalid sigma0");
    }

    if (p.T0 <= 0.0) {
        cerr << "Error: T0 must be positive." << endl;
        throw invalid_argument("Invalid T0");
    }

    if (p.dsigma <= 0.0 || p.dmu <= 0.0) {
        cerr << "Error: dsigma and dmu must be positive." << endl;
        throw invalid_argument("Invalid parameter step");
    }

    if (p.n_steps <= 0) {
        cerr << "Error: n_steps must be positive." << endl;
        throw invalid_argument("Invalid n_steps");
    }

    if (p.n_equil <= 0) {
        cerr << "Error: n_equil must be positive." << endl;
        throw invalid_argument("Invalid n_equil");
    }

    if (p.cooling <= 0.0 || p.cooling >= 1.0) {
        cerr << "Error: cooling must be between 0 and 1." << endl;
        throw invalid_argument("Invalid cooling");
    }

    if (p.delta <= 0.0) {
        cerr << "Error: delta must be positive." << endl;
        throw invalid_argument("Invalid delta");;
    }

    return p;
}