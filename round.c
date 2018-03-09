#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <sys/time.h>

int main(int argc, char **argv){

    MPI_Init(&argc, &argv);
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int ball = 1;
    if (rank == 0) {
        struct timeval stop, start;
        gettimeofday(&start, NULL);

        MPI_Send(&ball, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&ball, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        gettimeofday(&stop, NULL);
        int dt = (stop.tv_sec * 1000 + stop.tv_usec / 1000) - (start.tv_sec * 1000 + start.tv_usec / 1000);

        printf("Answer = %d by %dus\n", ball, dt);
        
    } else{
        MPI_Recv(&ball, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        ball ++;
        MPI_Send(&ball, 1, MPI_INT, rank == size - 1 ? 0 : rank + 1, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}