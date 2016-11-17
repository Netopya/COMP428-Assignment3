#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

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
    
    printf("Counted file\n");
    
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
    
    int i;
    for(i = 0; i < inputSize; i++)
    {
        printf("Input %d is %d\n",i,inputValue[i]);
    }
    
    free(inputValue);
}