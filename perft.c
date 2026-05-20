
#include <stdio.h>
#include <stdlib.h>
#include "Position.h"
#include <time.h>

int checks=0;
int mates=0;
int captures=0;
int count=0;

int sameMove(Move a,Move b)
{
    if(a.fromSq==b.fromSq && a.toSq==b.toSq && a.piece==b.piece && a.capture==b.capture)
    {
        return 1;
    }
    return 0;
}

void binary(uint8_t num)
{
    int i; printf("0");
    for(i=3;i>=0;i--)
    {
        printf("%d",(num>>i) & 1);
    }
    printf("\n");
}

int perft(Position* p, int depth)
{
    if(depth==0)
    {
        /*if(inCheck(p))
        {
            checks++;
        }*/
        /*if(p->movesMade[p->moveCount-1].capture!=EMPTY)
        {
            captures++;
        }*/
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
    //Position* p= readFen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq");
    //p->turn=0;
    binary(p->flags);
    display(&p->board[0]);
    int i;

    for(i=1;i<=5;i++)
    {
        int depth=i;
        checks=0; mates=0; captures=0;

        printf("%d positions at depth %d, ",perft(p,depth),depth);
        /*printf("%d checks found, ",checks);
        printf("%d checkmates ",mates);
        printf("%d captures\n", captures);*/
        clock_t end = clock();
        double ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
        printf("%f ms taken\n", ms);
    }

    return 0;
}

