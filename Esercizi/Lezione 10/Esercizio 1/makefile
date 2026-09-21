CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

RANDOM_PATH = ../../../Parallel\ Random\ Number\ Generator-20260727

all: main.exe

main.exe: Esercizio10_1.cpp ../../TSP.cpp ../../TSP.h ../../Cities.cpp ../../Cities.h $(RANDOM_PATH)/random.cpp
	$(CXX) $(CXXFLAGS) Esercizio10_1.cpp ../../TSP.cpp ../../Cities.cpp $(RANDOM_PATH)/random.cpp -o main.exe

clean:
	rm -f main.exe
