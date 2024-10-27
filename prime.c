%%sh
cat > prime.c << EOF
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char **argv) {
    int rank, total_p, upper_limit;
    double start_time , end_time , total_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &total_p);

    if (rank == 0) {
        printf("We want %d primes\n", total_p);
    }

    upper_limit = total_p;
    int c_prime = -1;  // Initialize each process's prime to -1 (unassigned)
    start_time = MPI_Wtime();

    
    if (rank == 0) {
        
        c_prime = 2;
        

        // Send non-multiples of 2 up to n
        for (int num = 3; num <= upper_limit * upper_limit; num+=2) {  // Increased range for more numbers
           
                MPI_Send(&num, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
            
        }

        // Define and send the terminator
        int terminator = -1;
        MPI_Send(&terminator, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);

        // Print the prime for Process 0
        printf("Process %d found prime: %d\n", rank, c_prime);

    } 
    else {
        int r_num;
        MPI_Recv(&r_num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int c_prime = r_num;  // Store the first received prime

        int number;
        while (1) {
            MPI_Recv(&number, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (number == -1) {  // Received terminator
                // Pass the terminator to the next process if not the last in the chain
                if (rank < total_p - 1) {
                    MPI_Send(&number, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                }
                break;
            }

            if (number % c_prime != 0) {  // Filter
                // Send to the next process if not the last
                if (rank < total_p - 1) {
                    MPI_Send(&number, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                }
            }
        }

        // Print the prime for each process
        printf("Process %d found prime: %d\n", rank, c_prime);
    }

    end_time = MPI_Wtime();
    total_time = end_time - start_time;

    if (rank == 0) {
        printf("Total time taken: %f seconds\n", total_time);
    }


    MPI_Finalize();

    return 0;
}
EOF
