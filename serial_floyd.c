#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
    // Open the input file
    FILE *myFile;
    myFile = fopen("input.txt", "r");
    
    int inputSize = 0;
    
    
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
    
    int n = sqrt(inputSize);
    
    printf("Square root of %d it %d\n", inputSize, n);
    
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
        printf("Read %d from file with a c of %d\n", inputValue[count], c);
        
        if(c != 1)
        {
            inputValue[count] = INT_MAX;
            char word[16];
            fscanf(myFile, "%s", &word);
        }
        
        count++;
    }
    
    int i,j,k;
    for(i = 0; i < inputSize; i++)
    {
        printf("Input %d is %d\n",i,inputValue[i]);
    }
    

    
    int* previousValues = inputValue;
    int* newValues = malloc(inputSize * sizeof(int));
    
    for(i = 0; i < inputSize; i++)
    {
        printf("Previous value %d is %d\n",i,previousValues[i]);
    }
    
    for(k = 0; k < n; k++)
    {
        for(i = 0; i < n; i++)
        {
            for(j = 0; j < n; j++)
            {                 
                printf("k is %d. n is %d, i is %d, j is %d. Looking a index %d with indexes %d and %d\n", k, n, i, j, coordinateToIndex(i,j,n), coordinateToIndex(i,k,n), coordinateToIndex(k,j,n));
                printf("The buffer is:\n");
                
                int l,m;
                for(l = 0; l < n; l++)
                {
                    for(m = 0; m < n; m++)
                    {
                        printf("%11d", previousValues[coordinateToIndex(m,l,n)]);
                    }
                    printf("\n");
                }
                
                printf("Comparing %d\n", previousValues[coordinateToIndex(i,j,n)]);
                printf("to %d + %d = %d\n", previousValues[coordinateToIndex(i,k,n)], previousValues[coordinateToIndex(k,j,n)], carefulIntAdd(previousValues[coordinateToIndex(i,k,n)], previousValues[coordinateToIndex(k,j,n)]));
                

                
                newValues[coordinateToIndex(i,j,n)] = min(previousValues[coordinateToIndex(i,j,n)], carefulIntAdd(previousValues[coordinateToIndex(i,k,n)], previousValues[coordinateToIndex(k,j,n)]));
                
                printf("New buffer is:\n");
                for(l = 0; l < n; l++)
                {
                    for(m = 0; m < n; m++)
                    {
                        printf("%11d", newValues[coordinateToIndex(m,l,n)]);
                    }
                    printf("\n");
                }
                
                //printf("Press enter to continue...\n");
                //getchar();
            }
        }
        free(previousValues);
        previousValues = newValues;
        newValues = malloc(inputSize * sizeof(int));
    }
    
    printf("The final buffer is:\n");
    for(k = 0; k < n; k++)
    {
        for(i = 0; i < n; i++)
        {
            printf("%11d", previousValues[coordinateToIndex(i,k,n)]);
        }
        printf("\n");
    }

    free(newValues);
    free(previousValues);
}