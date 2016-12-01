/*********************************************************************
Code for COMP 428 Assignment 3
Michael Bilinsky 26992358
This program performs the Floyd's All-Pairs Shortest Path algorithm on
    a graph in a parallel fashion with row and column pipe-lining
The graph must be specified in "input.txt" as an adjacency matrix
The results are written to "output.txt" as an adjacency matrix of
    the weights of the shortest paths
**********************************************************************/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include <sys/time.h>

#define MASTER 0        /* task ID of master task */

// http://stackoverflow.com/questions/3437404/min-and-max-in-c
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

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

    struct timeval start, end;

    // Record the start time
    gettimeofday(&start, NULL);

    MPI_Status status;

    /* Obtain number of tasks and task ID */
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);

    // Seed the random number generator
    unsigned seed =  (unsigned)(time(0));
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

        int input;
        while(!feof(myFile))
        {
            int c = fscanf(myFile, "%d\t", &input);

            if(c != 1)
            {
                char word[16];
                fscanf(myFile, "%s\t", &word);
            }

            inputSize++;
        }

        n = sqrt(inputSize);

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

            if(c != 1)
            {
                inputValue[count] = INT_MAX;
                char word[16];
                fscanf(myFile, "%s", &word);
            }

            count++;
        }

        fclose(myFile);
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

    // Find the corresponding k for a processor's dimension
    int kcounts[n];
    int ncount = 0;
    for(i = 0; i < n; i++)
    {
        if(displs[ncount] + scounts[ncount] == i)
            ncount++;

        kcounts[i] = ncount;
    }

    int point[2];
    indexToCoordinate(taskid, pn, point);

    // Place each process into two comm worlds
    // Each processor will be in a comm world with all the other
    //     processors in its row and another for its column
    int rowrank, rowSize;
    MPI_Comm row_comm;
    MPI_Comm_split(MPI_COMM_WORLD, point[1], point[0], &row_comm);
    MPI_Comm_rank(row_comm,&rowrank);
    MPI_Comm_size(row_comm,&rowSize);

    int colrank, colSize;
    MPI_Comm col_comm;
    MPI_Comm_split(MPI_COMM_WORLD, point[0], point[1], &col_comm);
    MPI_Comm_rank(col_comm,&colrank);
    MPI_Comm_size(col_comm,&colSize);

    int k, x, y;
    for(k = 0; k < n; k++)
    {
        // Find the processor dimension responsible for this k
        int k_in_p = kcounts[k];

        // Allocate the buffers
        int* rowbuffer = malloc(scounts[point[0]] * sizeof(int));
        int* colbuffer = malloc(scounts[point[1]] * sizeof(int));

        // If the processor's Y coordinate matches, then it
        // has a k corresponding to a row that needs to be send out
        if(point[1] == k_in_p)
        {
            for(i = 0; i < scounts[point[0]]; i++)
            {
                int index = coordinateToIndex(displs[point[0]] + i, k, n);
                rowbuffer[i] = inputValue[index];
            }
            
            MPI_Request request;
            
            for(i=0; i < pn; i++)
            {
                if(i == point[1])
                    continue;

                MPI_Isend(rowbuffer, scounts[point[0]], MPI_INT, i, k, col_comm, &request);
            }            
        }
        
        // If the processor's X coordinate matches, then it
        // has a k corresponding to a column that needs to be send out
        if(point[0] == k_in_p)
        {
            for(i = 0; i < scounts[point[1]]; i++)
            {
                int index = coordinateToIndex(k, displs[point[1]] + i, n);
                colbuffer[i] = inputValue[index];
            }
            
            MPI_Request request;
            
            for(i=0; i < pn; i++)
            {
                if(i == point[0])
                    continue;

                MPI_Isend(colbuffer, scounts[point[1]], MPI_INT, i, k, row_comm, &request);
            } 
        }

        if(point[1] != k_in_p)
        {
            MPI_Recv(rowbuffer, scounts[point[0]], MPI_INT, k_in_p, k, col_comm, MPI_STATUS_IGNORE);
        }
        
        if(point[0] != k_in_p)
        {
            MPI_Recv(colbuffer, scounts[point[1]], MPI_INT, k_in_p, k, row_comm, MPI_STATUS_IGNORE);
        }

        // Locally perform the Floyd's all pair calculation
        for(y = 0; y < scounts[point[1]]; y++)
        {
            for(x = 0; x < scounts[point[0]]; x++)
            {
                int real_x = x + displs[point[0]];
                int real_y = y + displs[point[1]];
                if(real_x == k || real_y == k || real_x == real_y)
                    continue;

                inputValue[coordinateToIndex(real_x,real_y,n)] =
                    min(inputValue[coordinateToIndex(real_x,real_y,n)],
                    carefulIntAdd(rowbuffer[x], colbuffer[y]));
            }
        }

        free(rowbuffer);
        free(colbuffer);
    }

    // The master will collect the data from all the processes
    if(taskid == MASTER)
    {
        for(i = 1; i < numtasks; i++)
        {
            // Find the coordinate of the process we are reading from
            int ppoint[2];
            indexToCoordinate(i, pn, ppoint);

            int receiveBufferSize = scounts[ppoint[0]] * scounts[ppoint[1]];
            int* receiveBuffer = malloc(receiveBufferSize * sizeof(int));

            MPI_Recv(receiveBuffer, receiveBufferSize, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for(y = 0; y < scounts[ppoint[1]]; y++)
            {
                for(x = 0; x < scounts[ppoint[0]]; x++)
                {
                    inputValue[coordinateToIndex(displs[ppoint[0]] + x, displs[ppoint[1]] + y, n)] = receiveBuffer[x + (y * scounts[ppoint[0]])];
                }
            }

            free(receiveBuffer);
        }

        FILE *myFile;
        myFile = fopen("output.txt", "w");

        // Write the result to output.txt
        int x,y;
        for(y = 0; y < n; y++)
        {
            for(x = 0; x < n; x++)
            {
                fprintf(myFile, "%d\t", inputValue[coordinateToIndex(x,y,n)]);
            }

            fprintf(myFile, "\n");
        }

        fclose(myFile);
    }
    else
    {
        // Non-master processes figure out the data they were responsible
        // for a transmit it the the master
        int sendBufferSize = scounts[point[0]] * scounts[point[1]];
        int* sendBuffer = malloc(sendBufferSize * sizeof(int));

        for(y = 0; y < scounts[point[1]]; y++)
        {
            for(x = 0; x < scounts[point[0]]; x++)
            {
                sendBuffer[x + (y * scounts[point[0]])] = inputValue[coordinateToIndex(displs[point[0]] + x, displs[point[1]] + y, n)];
            }
        }

        MPI_Send(sendBuffer, sendBufferSize, MPI_INT, MASTER, 0, MPI_COMM_WORLD);

        free(sendBuffer);
    }

    MPI_Finalize();

    if(taskid == MASTER)
    {
        // If the graph is small output it to the console
        if(n < 20)
        {
            printf("The final buffer is:\n");
            for(k = 0; k < n; k++)
            {
                for(i = 0; i < n; i++)
                {
                    printf("%11d", inputValue[coordinateToIndex(i,k,n)]);
                }
                printf("\n");
            }
        }

        gettimeofday(&end, NULL);

        // Measure the execution time
        float endtime = ((end.tv_sec * 1000000 + end.tv_usec)
                  - (start.tv_sec * 1000000 + start.tv_usec)) / 1000000.0;

        printf("Program with %d processes took %10.9f seconds\n", numtasks, endtime);
    }

    free(inputValue);

    return 0;
}