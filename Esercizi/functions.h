#ifndef __FUNCTIONS__
#define __FUNCTIONS__

#include <vector>
#include <string>
#include "../Parallel Random Number Generator-20260727/random.h"
#include "Results.h"

std::vector<double>  chi2(int n, int m, int t, Random& rnd);

double prob(std::vector<double> r, std::string type);

double psi(double x, double sigma, double mu);

double ddpsi(double x, double sigma, double mu);

double prob(double x, double sigma, double mu);

double energy(double x, double sigma, double mu);

Parameters parse_arguments(int argc, char* argv[]);

#endif