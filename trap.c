#include <mpi.h>
#include <stdio.h>
#include <math.h>

// Function to evaluate the curve (y = f(x))
float f(float x) {
    return x * x ; // Example: y = x^2
}

// Function to compute the area of a trapezoid
float trapezoid_area(float a, float b, float d) { 
    float area = 0;
    for (float x = a; x < b; x+=d) {
        area += f(x) + f(x+d);
    }
    
    return area * d * 0.5;
}

int main(int argc, char** argv) {
    int rank, size;
    float a = 0.0f, b = 1.0f;  // Limits of integration
    int n;
    float start, end, local_area, total_area;
    double time_start, time_end, parallel_time;
    double comp_start, comp_end, comm1_start, comm1_end, comm2_start, comm2_end;
    double comm_time, comp_time;
    
    MPI_Init(&argc, &argv); // Initialize MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get rank of the process
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get number of processes
    time_start = MPI_Wtime();
    if (rank == 0) {
        // Get the number of intervals from the user
        printf("Enter the number of intervals: ");
        fflush(stdout);
        scanf("%d", &n);
    }
    comm1_start = MPI_Wtime();
    // Broadcast the number of intervals to all processes
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    comm1_end = MPI_Wtime();
    
    comp_start = MPI_Wtime();
    // Calculate the interval size for each process
    float d = (b - a) / n; // delta
    float region = (b - a)/ size;
    
    // Calculate local bounds for each process
    start = a + rank * region;
    end = start + region;
    
    // Each process calculates the area of its subinterval
    local_area = trapezoid_area(start, end, d);
    comp_end = MPI_Wtime();

    comm2_start = MPI_Wtime();
    // Reduce all local areas to the total area on the root process
    MPI_Reduce(&local_area, &total_area, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
    comm2_end = MPI_Wtime();

    time_end = MPI_Wtime();
    comm_time = (comm1_end - comm1_start) + (comm2_end - comm2_start);
    comp_time = comp_end - comp_start;
    parallel_time = time_end - time_start;
    if (rank == 0) {
        printf("\nThe total area under the curve is: %f\n", total_area);
        printf("\nParallel execution time is %f \n", parallel_time);
        printf("\nCommunication time is %f ", comm_time);
        printf("\nComputation time is %f", comp_time);
        fflush(stdout);

    }
  
   
    MPI_Finalize(); // Finalize MPI
    return 0;
}