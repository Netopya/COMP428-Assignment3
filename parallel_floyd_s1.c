#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <limits.h>

#define MASTER 0        /* task ID of master task */

int coordinateToIndex(int x, int y, int n)
{
    return x + (y * n);
}

void indexToCoordinate(int i, int n, int* point)
{
    point[0] = i % n;
    point[1] = i / n;
}

int carefulIntAdd(int a, int b)
{
    if(a == INT_MAX || b == INT_MAX)
        return INT_MAX;

    return a + b;
}

int main (int argc, char *argv[])
{
int	taskid,	        /* task ID - also used as seed number */
	numtasks,       /* number of tasks */
	rc,             /* return code */
	i;

    // Start the timer
    clock_t begin = clock();;

    MPI_Status status;

    /* Obtain number of tasks and task ID */
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);

    // Seed the random number generator
    unsigned seed =  (unsigned)(time(0) + taskid);
    srand(seed);

    int pn = sqrt(numtasks);
    if(!((pn * pn) == numtasks))
    {
        if(taskid == MASTER)
            printf ("The number of processes must be a square number\n");

        MPI_Abort(MPI_COMM_WORLD, 1);
        return rc;
    }

    int* inputValue;
    int n;
    int inputSize = 0;

    // Only the master will deal with reading the file
    if(taskid == MASTER)
    {
        // Open the input file
        FILE *myFile;
        myFile = fopen("input.txt", "r");


        printf("Welcome\n");

        int input;
        while(!feof(myFile))
        {
            int c = fscanf(myFile, "%d", &input);

            if(c != 1)
            {
                char word[16];
                fscanf(myFile, "%s", &word);
            }

            inputSize++;
        }

        printf("Counted file size of %d\n", inputSize);

        n = sqrt(inputSize);

        //printf("Square root of %d it %d\n", inputSize, n);

        if(!((n * n) == inputSize))
        {
            printf("The input does not have a square number of values\n");
            return 1;
        }

        inputValue = malloc(inputSize * sizeof(int));



        rewind(myFile);

        int count = 0;
        while(!feof(myFile))
        {
            int c = fscanf(myFile, "%d", &inputValue[count]);
            //printf("Read %d from file with a c of %d\n", inputValue[count], c);

            if(c != 1)
            {
                inputValue[count] = INT_MAX;
                char word[16];
                fscanf(myFile, "%s", &word);
            }

            count++;
        }
    }

    // Broadcast the number of inputs
    MPI_Bcast(&inputSize, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

    if(pn > n)
    {
        if(taskid == MASTER)
            printf ("You select too many processors to solve the data\n");

        MPI_Abort(MPI_COMM_WORLD, 1);
        return rc;
    }

    // Non-master processes will need to create the sequence buffer for themselves
    if(taskid != MASTER)
    {
        inputValue = malloc(inputSize * sizeof(int));
    }

    // Broadcast the sequence to all processors
    MPI_Bcast(inputValue, inputSize, MPI_INT, MASTER, MPI_COMM_WORLD);


    int scounts[pn];    // The number of rows and columns a section along the axis responsible for
    int displs[pn];     // The x and y coordinate where a row or column section starts

    displs[0] = 0;

    // Calculate the sections of the table each process will be responsible for
    int baseCount = n / pn;
    int remainder = n % pn;

    for(i = 0; i < pn; i++)
    {
        scounts[i] = baseCount;

        if(i < remainder)
        {
            scounts[i]++;
        }

        if(i != 0)
        {
            displs[i] = displs[i - 1] + scounts[i - 1];
        }
    }

    int point[2];
    indexToCoordinate(taskid, pn, point);
    printf("Process %d is in section (%d, %d). X dpls %d and count %d. Y dpls %d and count %d.\n", taskid, point[0], point[1], displs[point[0]], scounts[point[0]], displs[point[1]], scounts[point[1]]);

    int x, y;
    for(k = 0; k < n; k++)
    {

    }
    free(inputValue);

    MPI_Finalize();
}