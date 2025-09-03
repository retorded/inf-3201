#include <stdio.h>
#include <string.h>
#include <mpi.h>

int main(){
    int nproc; // Number of process running the distributed program (-n argument of mpiexec)
    int myrank; // Rank of the current process

    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD,&nproc);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

    int result=-1;
    MPI_Reduce(&myrank,&result,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

    printf("Result of the reduce on rank %d is %d\n", myrank, result);
    
    MPI_Finalize();
    return(0);
}
