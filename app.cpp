// BSC4-DS Assignment 03 - Matrix Helper File
// Use this file to generate two matrices (A and B) to be multiplied
// in parallel in your assignment03

#include <stdio.h>
using namespace std;
#include <cmath>
#include <iostream>
#include <cstdlib>
#pragma warning(disable : 4996) //Blocks C4996 warning which is caused by FILE fopen function


// Can change matrix size to create any size square matrix
#define MATRIXSIZE 8
int msq = MATRIXSIZE * MATRIXSIZE;

//TODO: change to relevant name
#define MATRIXNAME "matB.dat"


void readMatrix() {
    cout << MATRIXNAME << " created:" << endl;
    int matrix[msq];

    FILE *input = fopen(MATRIXNAME, "r");
    
    if (!input) {
        cout << "No file found to read called " << MATRIXNAME << endl;
        return;
    }
    // read the file and store into matrix array
    fread(matrix, sizeof(int), msq, input);
    fclose(input);
    
    // iterate through the array to print
    for(int i = 0; i<msq; i++){
        cout << matrix[i] << " ";
        if ((i+1)% MATRIXSIZE == 0) cout << endl;
    }
}


// function that generates a matrix in binary form on disk
void generateMatrixFile() {
    // generate a matrix of values that need to be written to disk in the form of a one dimensional array
    //this will write out an NxN matrix (default size 8x8)
    int matrix[msq];
    
    srand(time(NULL));
    
    // get random numbers for the matrix
    for (int i = 0; i<msq; i++){
        matrix[i] = rand() % 10;
    }
    
    // open up a file for writing
    // TODO: Run twice to generate two separate .dat files (change MATRIXNAME)
    FILE *output = fopen(MATRIXNAME, "wb");
    
    if (!output) {
        cout << "File " << MATRIXNAME << " not found." << endl;
        return;
    }
    
    // do a simple fwrite to write the matrix to file
    fwrite(matrix, sizeof(int), msq, output);
    
    // close the file when we are finished writing
    fclose(output);
    
    // read the saved file and print to screen
    readMatrix();
    
}

double ColMatrixVectorMultiply(int n, double *a, double *b, double *x, MPI_Comm comm) {
  int i, j;
  int nlocal;
  double *px;
  double *fx;
  int npes, myrank;
  double product;
  MPI_Status status;

  MPI_Comm_size(MPI_COMM_WORLD, &npes);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  nlocal = n/npes;

  px = (double *)malloc(n*sizeof(double));
  fx = (double *)malloc(n*sizeof(double));
  
  for (int i = 0; i < n; i++) {
    px[i] = 0.0;
    for (int j = 0; i < nlocal; j++) {
      px[i] += a[i*nlocal+j]*b[j];
    }
  }

  MPI_Reduce(px, fx, n, MPI_DOUBLE, MPI_SUM, 0, comm);

  MPI_Scatter(fx, nlocal, MPI_DOUBLE, x, nlocal, MPI_DOUBLE, 0, comm);

  free(px);
  free(fx);

  return product;
}

// the main function of the program
int main(int argc, char** argv) {
    generateMatrixFile();

    // int world_size, world_rank;

    // MPI_Init(NULL, NULL);
    // MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    // MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
}
