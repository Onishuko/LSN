#ifndef MPI_EXCHANGE_H
#define MPI_EXCHANGE_H

#include <mpi.h>

#include "TSP.h"
#include "Results.h"
#include "../Parallel Random Number Generator-20260727/random.h"

bool exchange_mpi(TSP& tsp, Replica& replica, int partner_rank, Random& rnd, MPI_Comm comm);

#endif 