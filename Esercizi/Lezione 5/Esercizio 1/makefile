CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2

all: main.exe

main.exe: Esercizio5_1.cpp ../../Simulation.cpp ../../functions.cpp ../../../Parallel\ Random\ Number\ Generator-20260727/random.cpp
	$(CXX) $(CXXFLAGS) Esercizio5_1.cpp ../../Simulation.cpp ../../functions.cpp ../../../Parallel\ Random\ Number\ Generator-20260727/random.cpp -o main.exe

clean:
	rm -f main.exe