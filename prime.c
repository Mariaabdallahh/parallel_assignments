#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int rank, p, n;

    MPI_Init(&argc, &argv); // Initialize MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get rank of the process
    MPI_Comm_size(MPI_COMM_WORLD, &p); // Get number of processes

    if (rank == 0) {
        printf("Enter the upper limit of prime search (n): ");
        fflush(stdout);
        scanf("%d", &n);
    }

    // Broadcast the value of n to all processes
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int radical_n = (int)sqrt(n); // Only need to divide by primes up to √n
    int *primes = malloc((n + 1) * sizeof(int));

    // Initialize primes array with 1s
    for (int i = 2; i <= n; i++) {
        primes[i] = 1;
    }

    // Process 0 sends a number from divide_nums to other processes
    if (rank == 0) {
        for (int i = 2; i < p && i <= radical_n; i++) {
            MPI_Send(&i, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } 
    else {
        // Each process receives the number from divide_nums from process 0 from MPI_Send with tag 0 
        int number;
        MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received number: %d\n", rank, number);

        // Mark non-primes in the range [2, n] that are multiples of received numb
        for (int i = number*number; i<= n; i+=number) {
				      primes[i]=0;

        }
    }

    // all processed other than the last process send the updated prime array to second process
    if (rank != p - 1) {
        // Send primes array to the next process in the chain
        MPI_Send(primes, n + 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    }
    else {
        // Last process in the chain sends the primes array back to process 0
        MPI_Send(primes, n + 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    // Process 0 receives the final primes array
    if (rank == 0) {
        MPI_Recv(primes, n + 1, MPI_INT, p - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Print the prime numbers
        printf("Prime numbers up to %d:\n", n);
        for (int i = 2; i <= n; i++) {
            if (primes[i] == 1) {
                printf("%d ", i);
            }
        }
        printf("\n");
    }

   
    free(primes);
    MPI_Finalize();
    return 0;
}
