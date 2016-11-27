/*********************************************************************
Code for COMP 428 Assignment 3
Michael Bilinsky 26992358
This program generates a graph represented as an adjacency matrix which
    is written to a "input.txt" file
The number of nodes in the graph can be specified as an execution parameter
The rate at which nodes are not connected can be set with the DISCONNECT_RATE
    and DISCONNECT_RATE_BASE variables
The maximum weight for an edge can be specified with MAX_NUMBER. MAX_NUMBER
    must be greater than 0 in order to generate an interesting graph.
    (So that there are no 0 weights)
**********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_NUMBER 100
#define DISCONNECT_RATE 50
#define DISCONNECT_RATE_BASE 100

int main (int argc, char *argv[])
{
    // Seed the random number generator
    unsigned seed =  (unsigned)(time(0));
    srand(seed);
    
    if(argc < 2)
    {
        printf("You did not specify a size for the graph\n");
        return 1;
    }
    
    
    int graphSize = strtol(argv[1], NULL, 10);
    
    FILE *myFile;
    myFile = fopen("input.txt", "w");
    
    int x,y;
    for(y = 0; y < graphSize; y++)
    {
        for(x = 0; x < graphSize; x++)
        {   
            // The diagonals of an adjency matrix are 0
            if(x == y)
            {
                fprintf(myFile, "%d\t", 0);
                continue;
            }

            // Randomly determine if the next node pair is not connected
            int disconnectChance = rand() % DISCONNECT_RATE_BASE;
            
            if(disconnectChance > DISCONNECT_RATE)
                fprintf(myFile, "%d\t", (rand() % (MAX_NUMBER - 1)) + 1); // Set the weight of the edge
            else
                fprintf(myFile, "inf\t");
        }
         
        fprintf(myFile, "\n");
    }

    fclose(myFile);
}   


