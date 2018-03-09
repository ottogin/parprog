#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <inttypes.h>
#include <limits.h>
#include <mpi.h>
#include <sys/time.h>

long preprocess_input(int argc, char **argv){
    if (argc != 2)
    {
        printf("Bad usage: %s <number of terms>\n", argv[0]);
        return -1;
    }
    errno = 0;
    char* endptr;
    long val = strtol(argv[1], &endptr, 10);
    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
           || (errno != 0 && val == 0)) {
       printf("Too long number\n");
       return -1;
    }

    if (endptr == argv[1]) {
        printf("No digits were found\n");
        return -1;
    }
    if (*endptr != '\0')
    { 
        printf("Bad arguments. There are some letters\n");
        return -1;
    }

    return val;
}

int main(int argc, char **argv){
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    long n_terms = preprocess_input(argc, argv);

    if(n_terms == -1){
        MPI_Finalize();
        return 0;
    }

    // Get the number of processes
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Get the rank of the process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    struct timeval stop, start;
    gettimeofday(&start, NULL);

    long double result = 0.0;
    const long step = n_terms / size;
    const long h_bound = rank == size - 1 ? n_terms + 1 : (rank + 1) * step + 1;
    const long l_bound = rank * step + 1;
    const long double one = 1.0;

    // Compute our part 
    
    for (long i = l_bound; i < h_bound; i++){
        result += one / i;
    }

    // Combine all computed parts
    if (rank == 0) {
        long double recieved_val;
        for(int i = 1; i < size; i++){
            MPI_Recv(&recieved_val, 1, MPI_LONG_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //printf("R:%Lf\n", recieved_val);
            result += recieved_val;
        }
        
        gettimeofday(&stop, NULL);
        int dt = (stop.tv_sec * 1000 + stop.tv_usec / 1000) - (start.tv_sec * 1000 + start.tv_usec / 1000);
        printf("Answer = %Lf by %dus \n", result, dt);
        
    } else{
        MPI_Send(&result, 1, MPI_LONG_DOUBLE, 0, 0, MPI_COMM_WORLD);
        //printf("S:%Lf\n", result);
    }
    // Finalize the MPI environment.
    MPI_Finalize();

    return 0;
}