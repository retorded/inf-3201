#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>

#define M 4
#define N 4

/// Simple function to display a matrix
void PrintMatrix(double *matrix, int m, int n){
  for(int i=0;i<m;i++){
    for(int j=0;j<n;j++){
      printf("%02.0lf ",matrix[i*n+j]);
    }
    printf("\n");
  }
}

/// Simple function to display a vector
void PrintVector(double *vector, int n){
  for(int j=0;j<n;j++){
    printf("%02.0lf ",vector[j]);
  }
  printf("\n");
}

/// Parallel matrix vector multiply
void MatrixVectorMultiply(double *local_rows,
                          double *local_vect,
                          double *local_result,     // Will store the local_vect result
                          int row_per_proc,
                          int vect_length){

  // First collect all other process vector components from last MatrixVectorMultiply call:
  double *vect=calloc(sizeof(double)*vect_length,1);
  MPI_Allgather(local_vect,row_per_proc,MPI_DOUBLE,vect, row_per_proc,MPI_DOUBLE,MPI_COMM_WORLD);

  // Perform local multiplication and store the result in local_result
  for(int m=0;m<row_per_proc;m++){
    local_result[m] = 0;
    for(int n=0;n<vect_length;n++){
      local_result[m] += vect[n] * local_rows[m*vect_length+n];
    }
  }

  free(vect);
}

/// Entry point
int main(){
    int nproc; // Number of process running the distributed program (-n argument of mpiexec)
    int myrank; // Rank of the current process

    /* Define a matrix
      0  1  2  3
      4  5  6  7
      8  9  10 11
      12 13 14 15
     */
    double matrix[M*N]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    double vect[N]={1,2,3,4};

    // As usual
    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD,&nproc);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

    // Print Matrix and Vector
    if(myrank==0){
      printf("----- Matrix -----\n");
      PrintMatrix(matrix, M, N);
      printf("----- Vector -----\n");
      PrintVector(vect, N);
    }

    // We do a row-wise decomposition (column-wise or checkerboard are possible too)
    int row_per_proc=M/nproc;

    // Scatter matrix rows
    int entry_per_proc=row_per_proc*N; // Number of entries (integers) from matrix sent to each process
    double *local_rows=calloc(sizeof(double)*entry_per_proc,1); // Row received by the current process
    MPI_Scatter(matrix,entry_per_proc,MPI_DOUBLE,local_rows,entry_per_proc,MPI_DOUBLE,0, MPI_COMM_WORLD);

    // Scatter vector components
    double local_vect[N];
		// We use row_per_proc since each proc computes row_per_proc vector output
		// Thus in Allgather (see MatrixVectorMultiple) and Scatter we should send row_per_proc vector element
		MPI_Scatter(vect,row_per_proc,MPI_DOUBLE,local_vect,row_per_proc,MPI_DOUBLE,0, MPI_COMM_WORLD);

    // Perform computation
    double *local_result=calloc(sizeof(double)*row_per_proc,1);
    MatrixVectorMultiply(local_rows,local_vect,local_result,row_per_proc,N);

    // Now if you want to do another multiplication with the the same matrix with the last result vector
    //    MatrixVectorMultiply(local_rows,local_result,local_result,row_per_proc,N); // No need to scatter local_result (see Allgather)

    // Gather the result
    double result[N];
    MPI_Gather(local_result,row_per_proc,MPI_DOUBLE,result,row_per_proc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Show the result
    if(myrank==0){
      printf("----- Result -----\n");
      PrintVector(result,N);
    }

    // Free memory
    free(local_result);
    free(local_rows);

    // Never forget:
    MPI_Finalize();

    return(0);
}
