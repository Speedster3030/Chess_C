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
    if(depth==0)
    {
        //displayBits(p->attackMap);
        if(inCheck(p))
        {
            //display(&p->board[0]);
            checks++;
        }
        /*if(p->movesMade[p->moveCount-1].capture==ENPASS)
        {
            display(&p->board[0]);
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
        /*if(moves.list[i].capture==ENPASS)
        {
            display(&p->board[0]);
        }*/
        makeMove(p,&moves.list[i]);
        nodes+=perft(p,depth-1);
        unmakeMove(p);

    }

    if(c==0)
    {
        mates++;
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

    for(i=1;i<6;i++)
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
