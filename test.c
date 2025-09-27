#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "Position.h"


void displayBits(unsigned long long b)
{
    int i;
    for(i=0;i<64;i++)
    {
        if(b & (1ULL<<(63-i)))
        {
            printf("p ");
        }
        else
        {
            printf("* ");
        }
        if(i%8==7)
        {
            printf("\n");
        }
    }
    printf("\n");
}

int main()
{

    Position* p=new_Position();

    display(p);

    return 0;
}
