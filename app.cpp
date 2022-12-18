// BSC4-DS Assignment 03
// Ryan Talbot - 3001508

#include <stdio.h>
using namespace std;
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <mpi.h>
#pragma warning(disable : 4996) //Blocks C4996 warning which is caused by FILE fopen function

#define MATRIXA "matA.dat"
#define MATRIXB "matB.dat"

#define MATRIXSIZE 8

int msq = MATRIXSIZE * MATRIXSIZE;
int matrixA[64];
int matrixB[64];
int matrixC[64];
int aa[MATRIXSIZE], cc[MATRIXSIZE];

// Modified helper files
void generateMatrixFile() {
    // generate a matrix of values that need to be written to disk in the form of a one dimensional array
    //this will write out an NxN matrix (default size 8x8)
    srand(time(NULL));
    
    // get random numbers for the matrix
    for (int i = 0; i < msq; i++){
        matrixA[i] = rand() % 10;
        matrixB[i] = rand() % 10;
    }
    
    // do a simple fwrite to write the matrix to file
    // close the file when we are finished writing
    FILE *outputA = fopen(MATRIXA, "wb");
    if (!outputA) {
        cout << "File " << MATRIXA << " not found." << endl;
        return;
    }
    fwrite(matrixA, sizeof(int), msq, outputA);
    fclose(outputA);
    
    FILE *outputB = fopen(MATRIXB, "wb");
    if (!outputB) {
        cout << "File " << MATRIXB << " not found." << endl;
        return;
    }
    fwrite(matrixB, sizeof(int), msq, outputB);
    fclose(outputB);
}

void readMatrix() {
    cout << MATRIXA << " and "  << MATRIXB << " created:" << endl;
    int matrixA[msq];
    int matrixB[msq];

    FILE *inputA = fopen(MATRIXA, "r");
    if (!inputA) {
        cout << "No file found to read called " << MATRIXA << endl;
        return;
    }
    fread(matrixA, sizeof(int), msq, inputA);
    fclose(inputA);
    
    FILE *inputB = fopen(MATRIXB, "r");
    if (!inputB) {
        cout << "No file found to read called " << MATRIXB << endl;
        return;
    }
    fread(matrixB, sizeof(int), msq, inputB);
    fclose(inputB);
    
    // iterate through the array to print
    for(int i = 0; i<msq; i++){
        cout << matrixA[i] << " ";
        if ((i+1)% MATRIXSIZE == 0) cout << endl;
    }

    cout << "---" << endl;

    for(int j = 0; j<msq; j++){
        cout << matrixB[j] << " ";
        if ((j+1)% MATRIXSIZE == 0) cout << endl;
    }
}

// Methods needed for brief, created but not implemented.
// https://stackoverflow.com/a/14166417/12164665
void printMatrix(int** matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

// https://www.geeksforgeeks.org/program-dot-product-cross-product-two-vector/
int dotProduct(int* rowA, int* colB, int n) {
    int product = 0;
    for (int i = 0; i < n; i++) {
        product += rowA[i] * colB[i];
    }
    return product;
}

void multiplyStripe(int** stripeA, int** matrixB, int** stripeC, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            stripeC[i][j] = dotProduct(stripeA[i], matrixB[j], n);
        }
    }
}

void coordinator(int world_size) {
    readMatrix();
}

void participant(int world_rank, int world_size) {
    // cout << world_rank << endl;
}

int main(int argc, char** argv) {
    // MPI vars and setup
    int world_rank, world_size;
    int sum = 0;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Ensuring required args
    if (argc != 4) {
        printf("This application requires 4 args.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    } else {
        if (world_rank == 0) {
            printf("Matrices present along with correct args, starting computation.\n");
        }
    }

    generateMatrixFile();

    if (world_rank == 0) {
        coordinator(world_size);
    } else {
        participant(world_rank, world_size); 
    }

    // Scatter rows of first matrix to procs
    MPI_Scatter(matrixA, msq / world_size, MPI_INT, aa, msq / world_size, MPI_INT , 0 , MPI_COMM_WORLD);
    // Broadcast second matrix to all procs
    MPI_Bcast(matrixB, msq, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
  
    // Do multiplication
    for (int i = 0; i < MATRIXSIZE; i++) {
        for (int j = 0; j < MATRIXSIZE; j++) {
            sum = sum + aa[j] * matrixB[j];
        }
        cc[i] = sum;
        sum = 0;
    }

    MPI_Gather(cc, msq / world_size, MPI_INT, matrixC, msq / world_size, MPI_INT, 0, MPI_COMM_WORLD);
        
    MPI_Barrier(MPI_COMM_WORLD);

    // Print result
    if (world_rank == 0) {
        cout << "\nResult:\n" << endl;
        for(int i = 0; i < msq; i++){
            cout << matrixC[i] << " ";
            if ((i+1)% MATRIXSIZE == 0) cout << endl;
        }
    }

    return MPI_Finalize();
}

