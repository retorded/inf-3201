#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define N 10
#define NRECV 2

typedef struct Coordinate {
  double x;
  double y;
  double z;
} Coordinate;

void SetupDerivedType(MPI_Datatype* mytype, int myrank){
  // Dimension of each element in the struct
  int dim[3]={1,1,1};

  // Compute offsets of elements
  MPI_Aint offsets[3]; // Constains the offsets (in bytes) of each elements in the struct
  MPI_Aint xaddr,yaddr,zaddr;

  // Retrieve the address of each elements
  Coordinate dummy;
  MPI_Get_address(&dummy.x,&xaddr);
  MPI_Get_address(&dummy.y,&yaddr);
  MPI_Get_address(&dummy.z,&zaddr);
  // Now compute offsets (note double size 8 on my machine)
  offsets[0]=0; // No offset for the first value
  offsets[1]=yaddr-xaddr; // Offset of y in relation to x
  offsets[2]=zaddr-xaddr; // Offset of z in relation to x

  MPI_Datatype types[3]={MPI_DOUBLE,MPI_DOUBLE,MPI_DOUBLE};
  MPI_Type_create_struct(3,dim,offsets,types,mytype);
  MPI_Type_commit(mytype);

  printf("Derived type created (rank %d):\n  dimentions: [%d,%d,%d]\n  offsets: [%d,%d,%d]\n",myrank, dim[0],dim[1],dim[2],offsets[0],offsets[1],offsets[2]);
}

int main(){
    int nproc; // Number of process running the distributed program (-n argument of mpiexec)
    int myrank; // Rank of the current process
    Coordinate recvpoints[NRECV];
    Coordinate points[N];

    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD,&nproc);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

    MPI_Datatype MPI_Coordinate_type;
    SetupDerivedType(&MPI_Coordinate_type, myrank);

    if(myrank == 0){
      for(int i=0;i<N;i++){
        points[i].x=100+i; // X between in [100,200[
        points[i].y=200+i; // Y between in [100,300[
        points[i].z=300+i; // Z between in [100,400[
      }
    }

    // Important! nproc must be divisible by NRECV (otherwise use MPI_Scatterv)
    MPI_Scatter(points, NRECV, MPI_Coordinate_type, recvpoints
                , NRECV, MPI_Coordinate_type, 0,
                MPI_COMM_WORLD);

    printf("Rank %d received: [%.1f,%.1f,%.1f] and [%.1f,%.1f,%.1f]\n",myrank,recvpoints[0].x,recvpoints[0].y,recvpoints[0].z,recvpoints[1].x,recvpoints[1].y,recvpoints[1].z);


    // DO NOT FORGET TO FREE TYPE MEMORY
    MPI_Type_free(&MPI_Coordinate_type);
    MPI_Finalize();

    return(0);
}
