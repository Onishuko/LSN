# Numerical Simulation Laboratory

This repository contains the exercises developed for the Numerical Simulation Laboratory course.

Lessons 1–10 are implemented in C++, while Lessons 11–12 are implemented in Python.

## Compilation and execution

### Lessons 1–5

The exercises in Lessons 1–5 use a `Makefile` for compilation.

From the directory of the corresponding exercise, run:

    make

The generated executable can then be run according to the instructions of the exercise.

### Lesson 6

Lesson 6 uses the `run_temperatures.sh` Bash script to compile and run the simulations.

    chmod +x run_temperatures.sh
    ./run_temperatures.sh

### Lesson 7

The exercises in Lesson 7 use a `Makefile`.

    make

### Lesson 8

The first exercise uses a `Makefile`:

    make

The second exercise uses the `run.sh` Bash script:

    chmod +x run.sh
    ./run.sh

The parameters of the second exercise can be modified in the `config.sh` file before running the script.

### Lesson 9

The exercises in Lesson 9 use a `Makefile`.

    make

### Lesson 10

Lesson 10 uses the MPI library for parallel execution.

The program can be compiled with:

    /usr/bin/mpicxx -std=c++17 -Wall Esercizio10_2.cpp ../../TSP.cpp ../../Cities.cpp ../../Exchange.cpp ../../../Parallel\ Random\ Number\ Generator-20260727/random.cpp -o main

It can then be executed using `mpirun` with 10 MPI processes:

    /usr/bin/mpirun --oversubscribe -np 10 ./main

### Lessons 11–12

Lessons 11 and 12 are implemented in Python.

The corresponding Python scripts and notebooks can be executed using a Python environment with the required packages installed.
