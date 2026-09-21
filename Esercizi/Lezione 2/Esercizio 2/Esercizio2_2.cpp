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

int main(int argc, char* argv[]) {
    Random rnd;
   
    if(argc != 3){
      cerr << "Usage: " << argv[0] << " <M> number of throws <N> number of blocks" << endl;
      return 1;
    }

    int M = stoi(argv[1]);
    int N = stoi(argv[2]);

    if(M <= 0 || N <= 0 || M % N != 0){
      cerr << "Error: M must be positive, N must be positive, and M must be divisible by N." << endl;
      return 1;
    }

    rnd.SetRandom(rnd);

    Simulation s(M, N);

    int n_steps = 100; // Number of steps for the random walk

    // Define the function for the discrete random walks
    function<vector<double>(Random&)> disc_func = [n_steps](Random& rnd) {
        vector<double> r2(n_steps);

        double x = 0.0, y = 0.0, z = 0.0;
        for(int i = 0; i < n_steps; i++){
            int direction = static_cast<int>(rnd.Rannyu(0, 6)); // Randomly choose a direction
            switch(direction){
                case 0: x += 1.0; break;
                case 1: x -= 1.0; break;
                case 2: y += 1.0; break;
                case 3: y -= 1.0; break;
                case 4: z += 1.0; break;
                case 5: z -= 1.0; break;
            }
            r2[i] = x*x + y*y + z*z;   
        }
        return r2;
    };

    // Define the function for the continuous random walks
    function<vector<double>(Random&)> cont_func = [n_steps](Random& rnd) {
        vector<double> r2(n_steps);

        double x = 0.0, y = 0.0, z = 0.0;
        for(int i = 0; i < n_steps; i++){
            double theta = acos(1 - 2 * rnd.Rannyu()); // Random angle for uniform distribution on a sphere
            double phi = 2 * M_PI * rnd.Rannyu(); // Random azimuthal angle
            x += sin(theta) * cos(phi);
            y += sin(theta) * sin(phi);
            z += cos(theta);
            r2[i] = x*x + y*y + z*z; 
        }
        return r2;
    };

    function<double(double)> transform = [](double x) {
        return sqrt(x); // Return the square root of the distance
    };

    // Define the error transformation function
    function<double(double, double)> error_transform = [](double mean, double error) {

        if (mean <= 0.0) {
            return 0.0;
        }

        return error / (2.0 * sqrt(mean));
    };

    Results disc = s.run_RW(rnd, n_steps, disc_func, nullptr, transform, error_transform);
    Results cont = s.run_RW(rnd, n_steps, cont_func, nullptr, transform, error_transform);

    s.Print(disc.ave_rw_prog, "disc_ave_rw_prog.txt");
    s.Print(cont.ave_rw_prog, "cont_ave_rw_prog.txt");
    s.Print(disc.err_rw_prog, "disc_err_rw_prog.txt");
    s.Print(cont.err_rw_prog, "cont_err_rw_prog.txt");

    rnd.SaveSeed();
    return 0;
}