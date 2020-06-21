#include <mpi.h>
#include <vector>
#include <iostream>
#include <math.h>

#include "a.h"    

size_t n_choose_k(size_t n, size_t k){
    
    size_t result = 1;

    for (size_t ii=0; ii < k; ii++){
        result *= (n-ii) / (ii+1);
    }

    return result;
}


std::vector<size_t> split_tasks(size_t world_size, size_t world_rank,
                    size_t itotal, int bodies,
                    std::vector<int> &istart_worlds, std::vector<int> &icount_worlds){
// Function to approximately divide 1b/2b/3b tasks for MPI
//
// Usage:
// world_size / world_rank : total number of processes and current process id (from 0 to world_size-1)
// itotal : number of monomers 
// bodies : if it is to calculate 1b/2b/3b
// istart_worlds/icount_worlds : the begining id (inclusive) and the number of the monomer for each world rank after splitting
// 
// Return: None

    std::vector<size_t> n_sample_worlds;
    int icurrent(0), icounts_rank(0); 

    for (size_t ii = 0; ii < world_size; ii++){

        // determine the iend for this rank by approximating percentage 
        double iend_percentage = 1 - pow( double( double(world_size-ii-1) / world_size ), 1.0/double(bodies)) ;

        int iend_temp = int (itotal * iend_percentage);
        iend_temp = std::max( icurrent + 1, iend_temp);

        size_t n_sample_temp = n_choose_k(itotal - icurrent, bodies) - n_choose_k(itotal - iend_temp, bodies);
        

        istart_worlds.push_back(icurrent);
        icount_worlds.push_back(int(iend_temp - icurrent));
        n_sample_worlds.push_back(n_sample_temp);

        icurrent = iend_temp;
    }

    return n_sample_worlds;
}
















double calculate(std::vector<double> inputvector){



    // bool mpi_initialized_ = false;


    double result = 0.0;



// #if HAVE_MPI==1

    // mpi_initialized_ = true;

   // Initialize MPI
   MPI_Init(NULL, NULL);

   // Get the number of processes
   int world_size;
   MPI_Comm_size(MPI_COMM_WORLD, &world_size);

   // Get the rank of the process
   int world_rank;
   MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

   // Get the name of the processorS
   char processor_name[MPI_MAX_PROCESSOR_NAME];
   int name_len;
   MPI_Get_processor_name(processor_name, &name_len);


//    std::vector<size_t> index_this_rank(nummon_);
//    size_t index_size_this_rank;    


    std::vector<int> istart_worlds, icount_worlds;
    size_t itotal = inputvector.size();
    int bodies = 2;

    std::vector<size_t> n_sample_worlds = split_tasks(world_size, world_rank, itotal, bodies, istart_worlds, icount_worlds) ;

//    if (world_rank == 0) {
//        std::vector<std::vector<size_t> > indexes(world_size);
//        for (size_t i = 0, id = 0; i < nummon_; i++) {            
//            indexes[id++].push_back(i);
//            if (id == world_size) { id = 0 ; }
//        }
//        for (int i = 1; i < world_size; i++){
//            size_t index_send_size = indexes[i].size();
//            MPI_Send(&index_send_size, 1, MPI_UNSIGNED, i, 20, MPI_COMM_WORLD);
//            MPI_Send(&indexes[i][0], index_send_size, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD);
//            std::cout << "Sending indexes from processor 0 to " << i << std::endl; 
//        }
//    } else {

//        MPI_Recv(&index_size_this_rank, 1, MPI_UNSIGNED, 0, 20, MPI_COMM_WORLD,
//                 MPI_STATUS_IGNORE);

//        MPI_Recv(&index_this_rank[0], index_size_this_rank, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD,
//                 MPI_STATUS_IGNORE);
//        std::cout << "Process " << world_rank << " received matrix from process 0 " << std::endl;
	 		
//        for (size_t i = 0; i < index_size_this_rank; i++){
//            std::cout << "Rank " << world_rank << " calculates " << index_this_rank[i] << std::endl;
//        }

//    }

    






// #endif


    // Some functions that make calculations
    std::vector<double> temp_result(icount_worlds[world_rank], 0.0);
    std::vector<double>::iterator it = inputvector.begin();
    for (size_t ii = 0; ii < icount_worlds[world_rank]; ii++){
        temp_result[ii] = inputvector[istart_worlds[world_rank] + ii] * 0.1 + 0.001 * ii;
        result += temp_result[ii];
    }

    // MPI_Allreduce to get sum for all worlds
    double result_final(0.0);
    MPI_Allreduce( &result, &result_final, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);


    // MPI_Allgatherv to gather vectors for all worlds
    // ?? How to solve the problem of const int* ?


    double *result_recv = new double[itotal];


    MPI_Allgatherv( &temp_result[0],  icount_worlds[world_rank], MPI_DOUBLE,
                    result_recv, &icount_worlds[0], &istart_worlds[0], MPI_DOUBLE,
                    MPI_COMM_WORLD);



    for (int ii=0; ii<itotal; ii++){
        std::cout << world_rank << "\t" << result_recv[ii] << std::endl;
    }



    delete [] result_recv;

    // for ( std::vector<double>::iterator it = inputvector.begin(); it != inputvector.end(); it++){
    //     result += (*it) * 0.3 ; 

    // }



    // if (world_rank == 0){
    //     size_t world_size_test = 16;
    //     size_t itotal = 100000;
    //     size_t istart(0), iend(0), icounts_rank(0);

    //     for (size_t ii=0; ii<world_size_test; ii++){
    //         icounts_rank = split_tasks(world_size_test, ii, itotal, istart, iend, 2) ;

    //         std::cout << istart << " \t " << iend << " \t " << icounts_rank << std::endl;

    //     }

    // }



    return result_final;
}



int main(){

    std::vector<double> inbuff;

    size_t itotal = 20;

    for(size_t i=0; i<itotal; i++){
        double v =  2.1 * i ;
        inbuff.push_back( v );
    }

    double result = calculate(inbuff);

    std::cout << result << std::endl;




    return 0;
}