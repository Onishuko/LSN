#ifndef CITIES_h
#define CITIES_H

#include <vector>
#include <functional>
#include <string>

#include "../Parallel Random Number Generator-20260727/random.h"

using namespace std;

vector<vector<double>> generate_cities(Random& rnd, int N, const function<vector<double>(Random&)>& generator);

vector<vector<double>> read_cities(const string& filename, int N);

#endif