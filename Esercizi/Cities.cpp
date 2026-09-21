#include "Cities.h"
#include <iostream>
#include <fstream>

using namespace std;

/// @brief Generates a list of cities with random positions
/// @param rnd Random number generator
/// @param N Number of cities to generate
/// @param generator Function to generate random positions
/// @return Vector of city positions
vector<vector<double>> generate_cities(Random& rnd, int N, const function<vector<double>(Random&)>& generator){

    vector<vector<double>> cities;
    cities.reserve(N);

    for(int i = 0; i < N; i++){
        cities.push_back(generator(rnd));
    }

    return cities;
}

/// @brief Reads city positions from a file
/// @param filename Name of the file to read from
/// @param N Number of cities to read
/// @return Vector of city positions
vector<vector<double>> read_cities(const string& filename, int N){

    vector<vector<double>> cities(N, vector<double>(2));

    ifstream input(filename);

    if(!input.is_open()){
        cerr << "Unable to open " << filename << endl;
        return {};
    }

    for(int i = 0; i < N; i++){
        if(!(input >> cities[i][0] >> cities[i][1])){
            cerr << "Unable to read coordinates of province " << i << endl;
            return {};
        }
    }
    input.close();
    return cities;
}