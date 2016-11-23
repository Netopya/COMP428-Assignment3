#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_NUMBER 100
#define DISCONNECT_RATE 30
#define DISCONNECT_RATE_BASE 100

int main (int argc, char *argv[])
{
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
            if(x == y)
            {
                fprintf(myFile, "%d\t", 0);
                continue;
            }

            int disconnectChance = rand() % DISCONNECT_RATE_BASE;
            
            if(disconnectChance > DISCONNECT_RATE)
                fprintf(myFile, "%d\t", rand() % MAX_NUMBER);
            else
                fprintf(myFile, "inf\t");
        }
         
        fprintf(myFile, "\n");
    }

    fclose(myFile);
}   


