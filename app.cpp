// BSC4-DS Assignment 03 - Matrix Helper File
// Use this file to generate two matrices (A and B) to be multiplied
// in parallel in your assignment03

#include <stdio.h>
using namespace std;
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <mpi.h>
#pragma warning(disable : 4996) //Blocks C4996 warning which is caused by FILE fopen function

// Can change matrix size to create any size square matrix
// #define MATRIXSIZE 8
// int msq = MATRIXSIZE * MATRIXSIZE;
//
// #define MATRIXA "matA.dat"
// #define MATRIXB "matB.dat"
//
//int matrixA[MATRIXSIZE][MATRIXSIZE];
//int matrixB[MATRIXSIZE][MATRIXSIZE];
//int matrixC[MATRIXSIZE][MATRIXSIZE];
//int aa[8],cc[8];

// void readMatrix() {
//     cout << MATRIXA << " and " << MATRIXB << " created:" << endl;
//     
//     FILE *inputA = fopen(MATRIXA, "r");
//     FILE *inputB = fopen(MATRIXB, "r");
//     
//     if (!inputA || !inputB) {
//         cout << "No file found to read called " << MATRIXA << " or " << MATRIXB << endl;
//         return;
//     }
//     // read the file and store into matrix array
//     fread(matrixA, sizeof(int), msq, inputA);
//     fclose(inputA);
//     
//     fread(matrixB, sizeof(int), msq, inputB);
//     fclose(inputB);
//     
//     // iterate through the array to print
//     // for(int i = 0; i<msq; i++){
//     //     cout << matrixA[i][j] << " ";
//     //     if ((i+1)% MATRIXSIZE == 0) cout << endl;
//     // }
//     //
//     // cout << "---" << endl;
//     //
//     // for(int j = 0; j < msq; j++) {
//     //     cout << matrixB[j] << " ";
//     //     if ((j+1)% MATRIXSIZE == 0) cout << endl;
//     // }
// }
//
//
// // function that generates a matrix in binary form on disk
// void generateMatrixFile() {
//     // generate a matrix of values that need to be written to disk in the form of a one dimensional array
//     // this will write out an NxN matrix (default size 8x8)
//     int matrix_A[8][8];
//     int matrix_B[8][8];
//     
//     srand(time(NULL));
//     
//     // get random numbers for the matrix
//     for (int i = 0; i < msq; i++){
//         for (int j = 0; j < msq; j++) {
//             matrix_A[i][j] = rand() % 10;
//             matrix_B[j][i] = rand() % 10;
//         }
//     }
//     
//     // open up a file for writing
//     FILE *outputA = fopen(MATRIXA, "wb");
//     FILE *outputB = fopen(MATRIXB, "wb");
//     
//     if (!outputA || !outputB) {
//         cout << "File " << MATRIXA << " or " << MATRIXB << " not found." << endl;
//         return;
//     }
//     
//     // do a simple fwrite to write the matrix to file
//     fwrite(matrix_A, sizeof(int), msq, outputA);
//     fwrite(matrix_B, sizeof(int), msq, outputB);
//     
//     // close the file when we are finished writing
//     fclose(outputA);
//     fclose(outputB);
//     
//     // read the saved file and print to screen
//     //readMatrix();
// }

void print_results(char *prompt, int a[8][8])
{
    int i, j;

    printf ("\n\n%s\n", prompt);
    for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                    printf(" %d", a[i][j]);
            }
            printf ("\n");
    }
    printf ("\n\n");
}

// the main function of the program
int main(int argc, char** argv) {
    //generateMatrixFile();

    int matrixA[8][8] = {{1,2,3,4,1,6,7,3},{5,6,7,8,2,6,7,6},{9,1,2,3,7,3,2,6},{4,5,6,7,4,1,4,5},{1,2,3,4,1,6,7,3},{5,6,7,8,2,6,7,6},{9,1,2,3,7,3,2,6},{4,5,6,7,4,1,4,5}};
    int matrixB[8][8] = {{1,2,3,4,1,6,7,3},{5,6,7,8,2,6,7,6},{9,1,2,3,7,3,2,6},{4,5,6,7,4,1,4,5},{1,2,3,4,1,6,7,3},{5,6,7,8,2,6,7,6},{9,1,2,3,7,3,2,6},{4,5,6,7,4,1,4,5}};
    int matrixC[8][8];
    int aa[8], cc[8];

    int world_size, world_rank;

    int sum = 0;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
 
    //scatter rows of first matrix to different processes     
    MPI_Scatter(matrixA, 8*8/world_size, MPI_INT, aa, 8*8/world_size, MPI_INT,0,MPI_COMM_WORLD);

    //broadcast second matrix to all processes
    MPI_Bcast(matrixB, 8*8, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

          //perform vector multiplication by all processes
          for (int i = 0; i < 8; i++)
            {
                    for (int j = 0; j < 8; j++)
                    {
                            sum = sum + aa[j] * matrixB[j][i];  //MISTAKE_WAS_HERE               
                    }
                    cc[i] = sum;
                    sum = 0;
            }

    MPI_Gather(cc, 8*8/world_size, MPI_INT, matrixC, 8*8/world_size, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);        
    MPI_Finalize();

    if (world_rank == 0)                         //I_ADDED_THIS
        print_results("C = ", matrixC);

    return 0;
}
