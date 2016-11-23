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

int coordinateToIndex(int x, int y, int n)
{
    return x + (y * n);
}

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

    printf("Counted file size of %d\n", inputSize);

    int n = sqrt(inputSize);

    //printf("Square root of %d it %d\n", inputSize, n);

    if(!((n * n) == inputSize))
    {
        printf("The input does not have a square number of values\n");
        return 1;
    }

    int* inputValue = malloc(inputSize * sizeof(int));



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

    fclose(myFile);

    int i,j,k;

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

    /*printf("The final buffer is:\n");
    for(k = 0; k < n; k++)
    {
        for(i = 0; i < n; i++)
        {
            printf("%11d", inputValue[coordinateToIndex(i,k,n)]);
        }
        printf("\n");
    }*/

    free(inputValue);
    
    gettimeofday(&end, NULL);
    
    // Measure the execution time
    float endtime = ((end.tv_sec * 1000000 + end.tv_usec)
        - (start.tv_sec * 1000000 + start.tv_usec)) / 1000000.0;
    
    printf("\nProgram took %10.8f seconds\n",endtime);
}