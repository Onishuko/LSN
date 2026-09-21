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

    double l = 8.0; // stick length
    double a = 10.0; // distance between lines

    // Define the hit function for Buffon's needle problem
    function<double(Random&)> hitfunc = [l, a](Random& rnd) {
        
        double x, y, Y, r;
        double r2;

        do {
            x = rnd.Rannyu(-1.0, 1.0); 
            y = rnd.Rannyu(-1.0, 1.0); 
            
            r2 = x*x + y*y; // Calculate the squared distance from the origin to the point (x, y)

        } while (r2 > 1.0 || r2 == 0.0); // Ensure the point is inside the unit circle
        
        Y = abs(y)/sqrt(r2); // Calculate the sine of the angle between the stick and the horizontal axis
        r = rnd.Rannyu(0.0, a/2.0); // Random distance from the center of the stick to the nearest line

        return (r <= l/2.0 * Y) ? 1.0 : 0.0; // Return 1 if the stick crosses a line, otherwise return 0
    };

    // Define the mean function to estimate the value of pi 
    function <double(double)> meanfunc = [l, a](double mean) {

        if (mean == 0.0) {
            return 0.0; // Avoid division by zero
        }

        return 2.0 * l / (a * mean); // Calculate the estimated value of pi based on the mean
    };

    // Define the error transformation function to propagate the error in the estimation of pi
    function<double(double, double)> error_transform = [l, a](double mean, double error) {

        if (mean == 0.0) {
            return 0.0;
        }

        return (2.0 * l / (a * mean * mean)) * error;
    };

    Results results = s.run(rnd, hitfunc, nullptr, meanfunc, error_transform);

    s.Print(results.ave_prog, "ave_prog.txt");
    s.Print(results.err_prog, "err_prog.txt");


    rnd.SaveSeed();
    return 0;
}