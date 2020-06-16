#include <mpi.h>
#include <vector>
#include <iostream>


size_t split_tasks( size_t world_size, size_t world_rank, 
                    size_t itotal, size_t &istart, size_t &iend,
                    int bodies){
    size_t n_total, n_rank;  // n_total: total pairs/triples; n_rank: number of pairs/triples at this rank

    if (bodies == 2) {
        n_total = itotal*(itotal-1) / 2;
    } else if ( bodies == 3){
        n_total = itotal*(itotal-1)*(itotal-2) / 6;
    } else{
        std::cerr << "Wrong input: bodies should be either 2 or 3.";
    }

    size_t n_ave = size_t (n_total/world_size); // expected average n_total for each rank;

    std::vector<size_t> istart_all;
    std::vector<size_t> iend_all;
    std::vector<size_t> n_rank_all;
    size_t i_current = 0; // 

    for(size_t irank = 0; irank < world_size ; irank++){
        istart_all.push_back(ii)
        size_t n_rank = 0;
        n_i = 0;

        // check how many samples for current i
        {
            n_rank += n_i;
            if (bodies == 2) {
                n_i = size_t ( itotal - istart - 1 );
            } else {
                n_i = size_t ( itotal - istart - 1) * (itotal - istart - 2) / 2;
            }
        }while (n_rank + n_i < n_ave)




    } 





    return iworld;

}




double calculate(std::vector<double> input_){



    bool mpi_initialized_ = false;


    double result = 0.0;



#if HAVE_MPI==1

    mpi_initialized_ = true;

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

    






#endif


    

    for ( std::vector<double>::iterator it = input_.begin(); it != input_.end(); it++){
        result += (*it) * 0.3 ; 

    }



    return result;
}



int main(){

    std::vector<double> inbuff;

    for(int i=0; i<100; i++){
        double v =  2.1 * i ;
        inbuff.push_back( v );
    }

    double result = calculate(inbuff);

    std::cout << result << std::endl;


    return 0;
}