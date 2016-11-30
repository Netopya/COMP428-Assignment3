/*********************************************************************
Code for COMP 428 Assignment 3
Michael Bilinsky 26992358
This program performs the Floyd's All-Pairs Shortest Path algorithm on
    a graph in a serial fashion
The graph must be specified in "input.txt" as an adjacency matrix
The results are written to "output.txt" as an adjacency matrix of
    the weights of the shortest paths
**********************************************************************/
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

// http://stackoverflow.com/questions/3437404/min-and-max-in-c
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

// Convert an x, y coordinate on a graph of width n into
//     an index representing the position of that coordinate
//     within an array representation of the graph
int coordinateToIndex(int x, int y, int n)
{
    return x + (y * n);
}

// Carefully add two numbers, if one of the numbers is infinity,
//     don't add and set the result infinity directly to 
//     prevent overflow
int carefulIntAdd(int a, int b)
{
    if(a == INT_MAX || b == INT_MAX)
        return INT_MAX;

    return a + b;
}

int main (int argc, char *argv[])
{
    struct timeval start, end;

    // Record the start time
    gettimeofday(&start, NULL);
    
    // Open the input file
    FILE *myFile;
    myFile = fopen("input.txt", "r");

    int inputSize = 0;


    printf("Welcome\n");

    int input;
    
    //Count the number of items in the input file
    while(!feof(myFile))
    {
        // Look for a number
        int c = fscanf(myFile, "%d\t", &input);

        // If in is not a number it may be an infinity string
        if(c != 1)
        {
            char word[16];
            fscanf(myFile, "%s\t", &word);
        }

        inputSize++;
    }

    printf("Counted file size of %d\n", inputSize);

    int n = sqrt(inputSize);

    // Confirm that there are a valid number of items
    if(!((n * n) == inputSize))
    {
        printf("The input does not have a square number of values\n");
        return 1;
    }

    // Create the buffer to store the input values
    int* inputValue = malloc(inputSize * sizeof(int));

    rewind(myFile);

    // Read input from the file
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

    int i,j,k;

    // Perform the Floyd's all pair algorithm
    for(k = 0; k < n; k++)
    {
        for(i = 0; i < n; i++)
        {
            for(j = 0; j < n; j++)
            {
                if(i == k || j == k || i == j)
                    continue;

                inputValue[coordinateToIndex(i,j,n)] = min(inputValue[coordinateToIndex(i,j,n)], carefulIntAdd(inputValue[coordinateToIndex(i,k,n)], inputValue[coordinateToIndex(k,j,n)]));
            }
        }
    }

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
    
    free(inputValue);
    
    gettimeofday(&end, NULL);
    
    // Measure the execution time
    float endtime = ((end.tv_sec * 1000000 + end.tv_usec)
        - (start.tv_sec * 1000000 + start.tv_usec)) / 1000000.0;
    
    printf("\nProgram took %10.9f seconds\n",endtime);
    
    return 0;
}