#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define BUFFER_SIZE 100

int main(int argc, char **argv){
    char buffer[BUFFER_SIZE];
    int nproc; // Number of process running the distributed program (-n argument of mpiexec)
    int myrank; // Rank of the current process

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nproc);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

    if(myrank==0){
        printf("Rank %d: Collecting messages...\n",myrank);
        for(int i=1;i<nproc;i++){
            // Recv(<data>,<size>,<type>,<src>,<tag>,<comm>,<status>)
            MPI_Recv(buffer,BUFFER_SIZE,MPI_CHAR,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            printf("Rank %d: %s", myrank, buffer);
        }
    }
    else {
        sprintf(buffer,"This is a message from rank %d\n",myrank);
        // Send(<data>,<size>,<type>,<dst>,<tag>,<comm>)
        MPI_Send(buffer,strlen(buffer)+1,MPI_CHAR,0,0,MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return(0);
}
