#include <stdio.h>
#include <stdlib.h>
#include "Position.h"
#include <time.h>

int checks=0;
int mates=0;

int sameMove(Move a,Move b)
{
    if(a.fromSq==b.fromSq && a.toSq==b.toSq && a.piece==b.piece && a.capture==b.capture)
    {
        return 1;
    }
    return 0;
}

int perft(Position* p, int depth)
{
    if(depth==1)
    {
        if(inCheck(p))
        {
            checks++;
        }

        moveList moves;
        getMoves(p,&moves);
        return moves.count;
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
    //Position* p= new_Position();
    Position* p= readFen("r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq ");
    //p->turn=0;
    display(&p->board[0]);
    int i;

    for(i=1;i<=4;i++)
    {
        int depth=i;
        checks=0; mates=0;

        printf("%d positions found at depth %d, ",perft(p,depth),depth);
        printf("%d checks found\n",checks);
        printf("%d checkmates\n",mates);
        clock_t end = clock();
        double ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
        printf("%f ms taken\n", ms);
    }

    return 0;
}
