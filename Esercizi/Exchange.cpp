#include "Exchange.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

using namespace std;

/// @brief Performs the exchange of configurations between two replicas in a parallel tempering simulation using MPI
/// @param tsp The TSP object containing the problem definition  
/// @param replica The replica whose configuration is to be exchanged
/// @param partner_rank The rank of the partner replica
/// @param rnd The random number generator
/// @param comm The MPI communicator
/// @return A boolean indicating whether the exchange was accepted
bool exchange_mpi(TSP& tsp, Replica& replica, int partner_rank, Random& rnd, MPI_Comm comm){

    int rank;
    MPI_Comm_rank(comm, &rank);

    // Exchange paths
    int N = static_cast<int>(replica.path.path.size());
    vector<int> partner_path(N);

    MPI_Sendrecv(replica.path.path.data(), N, MPI_INT, partner_rank, 0, partner_path.data(), N, MPI_INT, partner_rank, 0, comm, MPI_STATUS_IGNORE); //.data() restituisce un puntatore al primo elemento del path

    // Exchange costs
    double partner_cost;

    MPI_Sendrecv(&replica.path.cost, 1, MPI_DOUBLE, partner_rank, 1, &partner_cost, 1, MPI_DOUBLE, partner_rank, 1, comm, MPI_STATUS_IGNORE);

    // Exchange temperature
    double partner_temperature;

    MPI_Sendrecv(&replica.T, 1, MPI_DOUBLE, partner_rank, 2, &partner_temperature, 1, MPI_DOUBLE, partner_rank, 2, comm, MPI_STATUS_IGNORE);

    // Construct partner
    Replica partner;
    partner.path.path = partner_path;
    partner.path.cost = partner_cost;
    partner.T = partner_temperature;

    int decision = 0;

    // Lower rank perform exchange() [abitrary decision]

    if(rank < partner_rank){

        bool accepted = tsp.exchange(replica, partner, rnd);

        decision = accepted ? 1 : 0;

        // Send decision
        MPI_Send(&decision, 1, MPI_INT, partner_rank, 3, comm);

        //If accepted send new configuration belonging to the partner
        if(decision == 1){

            MPI_Send(partner.path.path.data(), N, MPI_INT, partner_rank, 4, comm);
            MPI_Send(&partner.path.cost, 1, MPI_DOUBLE, partner_rank, 5, comm);
        
        }
    } else {

        // Higher rank recieves the decision
        MPI_Recv(&decision, 1, MPI_INT, partner_rank, 3, comm, MPI_STATUS_IGNORE);

        //If accepted recieve new configuration
        if(decision == 1){
            
            MPI_Recv(replica.path.path.data(), N, MPI_INT, partner_rank, 4, comm, MPI_STATUS_IGNORE);
            MPI_Recv(&replica.path.cost, 1, MPI_DOUBLE, partner_rank, 5, comm, MPI_STATUS_IGNORE);
        }
    }

    return decision == 1;

}