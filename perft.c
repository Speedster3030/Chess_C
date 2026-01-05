#include <stdio.h>
#include <stdlib.h>
#include "Position.h"
#include <time.h>

int perft(Position* p, int depth)
{
    if(depth==0)
    {
        //displayBits(p->whitePieces);
        /*if(inCheck(p))
        {
            display(p);
        }*/
        /*moveList moves;
        getMoves(p,&moves);*/
        return 1;//moves.count;
    }
    moveList moves;
    getMoves(p,&moves);
    int i,nodes=0;
    int c=moves.count;

    for(i=0;i<c;i++)
    {

        makeMove(p,&moves.list[i]);
        nodes+=perft(p,depth-1);
        unmakeMove(p);

    }

    return nodes;
}

int main()
{
    clock_t start = clock();
    Position* p= new_Position();
    //p->turn=0;
    //display(p);
    int i;

    for(i=1;i<5;i++)
    {
        int depth=i;

        printf("%d positions found at depth %d, ",perft(p,depth),depth);
        clock_t end = clock();
        double ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
        printf("%f ms taken\n", ms);
    }

    return 0;
}
