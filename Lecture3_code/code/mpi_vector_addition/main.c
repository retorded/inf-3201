#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define VECT_LENGTH 3

void ReadVector(double *vect){
  printf("Enter first vector (length=%d):\n",VECT_LENGTH);
  for(int i=0;i<VECT_LENGTH;i++){
    scanf("%lf",&vect[i]);
  }
}

int main(){
    int nproc; // Number of process running the distributed program (-n argument of mpiexec)
    int myrank; // Rank of the current process
    double send_buffer[VECT_LENGTH];
    double v1;
    double v2;
    
    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD,&nproc);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

    if(myrank == 0)
      ReadVector(send_buffer);
    // MPI_Scatter(<SendBuffer>,<HowManySendPerProcess>,<SendBufferElementsType>,<ReceivedBuffer>,<HowManyToReceive>,<ReceivedBufferElementType>,<Src>,<COMM>)
    MPI_Scatter(send_buffer, 1, MPI_DOUBLE, &v1, 1, MPI_DOUBLE,0, MPI_COMM_WORLD);

    if(myrank == 0)
      ReadVector(send_buffer); // From stdin
    // MPI_Scatter(<SendBuffer>,<HowManySendPerProcess>,<SendBufferElementsType>,<ReceivedBuffer>,<HowManyToReceive>,<ReceivedBufferElementType>,<Src>,<COMM>)
    MPI_Scatter(send_buffer, 1, MPI_DOUBLE, &v2, 1, MPI_DOUBLE,0, MPI_COMM_WORLD);
    
    // Print received vectors
    printf("Rank %d received %lf and %lf\n",myrank,v1,v2);

    // Perform the sum
    double result=v1+v2;

    // Gather on rank 0
    double receive_buffer[VECT_LENGTH];
    // MPI_Gather(<SendBuffer>,<HowManySendPerProcess>,<SendBufferElementsType>,<ReceivedBuffer>,<HowManyToReceive>,<ReceivedBufferElementType>,<Root>,<COMM>)
    MPI_Gather(&result, 1, MPI_DOUBLE, receive_buffer, 1, MPI_DOUBLE,0, MPI_COMM_WORLD);
    if(myrank==0)
      printf("Rank %d received the result: %lf %lf %lf\n",myrank,receive_buffer[0],receive_buffer[1],receive_buffer[2]);

    MPI_Finalize();

    return(0);
}
