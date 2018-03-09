#include <stdio.h>
#include <mpi.h>
#include <sys/time.h>

int main(int argc, char** argv){

	struct timeval stop, start;
	gettimeofday(&start, NULL);
	
	// Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Print off a hello world message
    gettimeofday(&stop, NULL);
    int dt = (stop.tv_sec * 1000 + stop.tv_usec / 1000) - (start.tv_sec * 1000 + start.tv_usec / 1000);
    printf("%dus : Hello world! Rank %d out of %d processors\n", dt, world_rank, world_size);

    // Finalize the MPI environment.
    MPI_Finalize();

	return 0;
}