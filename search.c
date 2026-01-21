

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "Position.h"

void orderMoves(moveList* m);

int eval(Position* p)
{
    if(inCheck(p)){
    moveList moves;
    getMoves(p,&moves);
    if(moves.count==0)
    {
        return p->turn?-999:999;
    }
    }
    int i,sum=0;
    for(i=0;i<64;i++)
    {
        sum+=p->board[BOARD_SQ(i)];
    }

    return sum;
}

int minimax(Position* p,int depth,int alpha,int beta,int maximizing)
{
    if(depth==0)
    {
        return eval(p);
    }

    moveList moves;
    getMoves(p,&moves);
    orderMoves(&moves);
    int i;

    if(maximizing)
    {
        int best=-1000;
        for(i=0;i<moves.count;i++)
        {
            makeMove(p,&moves.list[i]);
            int e= minimax(p,depth-1,alpha,beta,0);
            unmakeMove(p);

            if(e>=beta) return beta;
            if(e>alpha) alpha=e;
            best=alpha;
        }
        return best;
    }
    else
    {
        int best=1000;
        for(i=0;i<moves.count;i++)
        {
            makeMove(p,&moves.list[i]);
            int e= minimax(p,depth-1,alpha,beta,1);
            unmakeMove(p);

            if(e<=alpha) return alpha;
            if(e<beta) beta=e;
            best=beta;
        }
        return best;
    }
}

void orderMoves(moveList* moves)
{
    int i,end=0;
    for(i=0;i<moves->count;i++)
    {
        if(moves->list[i].capture!=EMPTY)
        {
            Move t=moves->list[end];
            moves->list[end]=moves->list[i];
            moves->list[i]=t;
            end++;
        }
    }
}

int main()
{
    //Position* p=readFen("2k1R3/6R1/8/2b5/1q6/8/8/5K2 b HAha - 0 1");
    Position* p= new_Position();
    int i,val,n=0;
    clock_t start, end;
    moveList moves;

    while(p->moveCount<MAXGAMEMOVES)
    {
        if(p->turn)
        {
            start= clock();
            val=-1000;
            getMoves(p,&moves);
            if(moves.count==0)
            {
                break;
            }
            for(i=0;i<moves.count;i++)
            {
                makeMove(p,&moves.list[i]);
                int e=minimax(p,3,-5000,5000,0);
                n=(val<=e)?i:n;
                val=(val<=e)?e:val;
                unmakeMove(p);
            }
            makeMove(p,&moves.list[n]);
            end= clock();
            double time= (double)(end-start) /CLOCKS_PER_SEC;
            printf("%.3f s taken\n",time);
        }
        else
        {
            display(&p->board[0]);
            getMoves(p,&moves);
            if(moves.count==0)
            {
                break;
            }
            for(i=0;i<moves.count;i++)
            {
                printf("%d: %d from %d to %d\n",i+1,moves.list[i].piece,moves.list[i].fromSq,moves.list[i].toSq);
            }
            int x;
            scanf("%d",&x);
            makeMove(p,&moves.list[x-1]);
        }
    }


    return 0;
}
