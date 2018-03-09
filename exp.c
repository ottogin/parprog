#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <inttypes.h>
#include <math.h>
#include <limits.h>
#include <mpi.h>
#include <sys/time.h>

long preprocess_input(int argc, char **argv, float* x){
    if (argc != 3)
    {
        printf("Bad usage: %s <number of signs> <x>\n", argv[0]);
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

    errno = 0;

    float val2 = strtof(argv[2], &endptr);
    if (errno == ERANGE) {
       printf("Bad 2nd arg\n");
       return -1;
    }
    *x = val2;

    return val;
}

int main(int argc, char **argv){
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    float x;
    long n_signs = preprocess_input(argc, argv, &x);

    if(n_signs == -1){
        MPI_Finalize();
        return 0;
    }

    // Compute n_terms
    long  n_terms = n_signs;
    /*long double temp1 = 1;
    long double temp2 = powl(10, -n_signs - 10);
    while(temp1 > temp2){
        temp1 *= x / n_terms;
        n_terms ++;
    }
    n_terms += 100;*/

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

    // Compute our part 
    long i = l_bound;
    long double last_term = 1.0;
    for (; i < h_bound; i++){
        result += last_term;
        last_term *= x / i;
    }

    // Combine all computed parts
    long double recieved_val;
    
    if (rank == 0) {
        
        if(size != 1){
            MPI_Recv(&recieved_val, 1, MPI_LONG_DOUBLE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            result += last_term * x / i * recieved_val;
        }
        
        gettimeofday(&stop, NULL);
        int dt = (stop.tv_sec * 1000 + stop.tv_usec / 1000) - (start.tv_sec * 1000 + start.tv_usec / 1000);
        printf("Took %dus : Answer = %.20Lf\n", dt, result);
        
    } else if(rank != size - 1){
        MPI_Recv(&recieved_val, 1, MPI_LONG_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        result += last_term * x / i * recieved_val;
        MPI_Send(&result, 1, MPI_LONG_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
        //printf("S:%Lf\n", result);
    } else{
        MPI_Send(&result, 1, MPI_LONG_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
    }
    // Finalize the MPI environment.
    MPI_Finalize();

    return 0;
}