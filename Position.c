/* started 27 Aug 2025; */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "Position.h"

void delete_Container(array_Container *ac)
{
    free(ac->moves);
    free(ac);
}

void setEdges(Position *p)
{
    int i;
    for(i=1;i<=8;i++)
    {
        p->board[i]=EDGE;
        p->board[i+10]=EDGE;
    }
    for(i=101;i<=108;i++)
    {
        p->board[i]=EDGE;
        p->board[i+10]=EDGE;
    }
    for(i=0;i<=110;i+=10)
    {
        p->board[i]=EDGE;
    }
    for(i=9;i<=119;i+=10)
    {
        p->board[i]=EDGE;
    }
}

void setBoard(Position *p)
{
    int i;
    p->board[21]=BLACKROOK;  p->board[28]=BLACKROOK;
    p->board[22]=BLACKKNIGHT;p->board[27]=BLACKKNIGHT;
    p->board[23]=BLACKBISHOP;p->board[26]=BLACKBISHOP;
    p->board[24]=BLACKQUEEN; p->board[25]=BLACKKING;
    p->board[98]=WHITEROOK;  p->board[91]=WHITEROOK;
    p->board[92]=WHITEKNIGHT;p->board[97]=WHITEKNIGHT;
    p->board[93]=WHITEBISHOP;p->board[96]=WHITEBISHOP;
    p->board[94]=WHITEQUEEN; p->board[95]=WHITEKING;
    for(i=31;i<=38;i++)
    {
        p->board[i]=BLACKPAWN;
    }
    for(i=81;i<=88;i++)
    {
        p->board[i]=WHITEPAWN;
    }
    p->whitePieces=0xffff;
    p->blackPieces=0xffff000000000000;
    p->turn=1;
}

void makeMove(Position *p,Move *m)
{
    p->board[m->toSq]=p->board[m->fromSq];
    p->board[m->fromSq]=EMPTY;
    int f=BIT_SQ(m->fromSq), t=BIT_SQ(m->toSq);
    //macro value directly in expressions gives errors
    if(p->turn)
    {
        p->whitePieces&=~(1ULL<<(63-f));
        p->whitePieces|=(1ULL<<(63-t));
        if(m->capture!=0)
        {
            p->blackPieces&=~(1ULL<<(63-t));
        }
    }
    else
    {
        p->blackPieces&=~(1ULL<<(63-f));
        p->blackPieces|=(1ULL<<(63-t));
        if(m->capture!=0)
        {
            p->whitePieces&=~(1ULL<<(63-t));
        }
    }
    p->movesMade[p->moveCount]=*m;
    p->moveCount++;
    p->turn=!p->turn;
}

void unmakeMove(Position *p)
{
    if(p->moveCount==0)
    {
        printf("ERROR! NO MOVES TO UNDO\n");
        return;
    }
    Move m=p->movesMade[p->moveCount-1];
    p->board[m.toSq]=m.capture;
    p->board[m.fromSq]=m.piece;
    int f=BIT_SQ(m.fromSq), t=BIT_SQ(m.toSq);
    if(p->turn)
    {
        p->blackPieces&=~(1ULL<<(63-t));
        p->blackPieces|=1ULL<<(63-f);
        if(m.capture!=0)
        {
            p->whitePieces|=1ULL<<(63-t);
        }
    }
    else
    {
        p->whitePieces&=~(1ULL<<(63-t));
        p->whitePieces|=1ULL<<(63-f);
        if(m.capture!=0)
        {
            p->blackPieces|=1ULL<<(63-t);
        }
    }
    p->moveCount--;
    p->turn=!p->turn;
}

array_Container* knightMoves(Position *p,int sq)
{
    Move* moves=malloc(MAXKNIGHTMOVES*sizeof(Move));
    array_Container *ac=malloc(sizeof(array_Container));
    if(ac==NULL)
    {
        printf("No memory for knight moves\n");
        exit(1);
    }

    int i,c=0,color=p->board[sq]>0?1:-1;
    int squares[MAXKNIGHTMOVES]={-21,-19,-12,-8,8,12,19,21};
    for(i=0;i<MAXKNIGHTMOVES;i++)
    {
        int s=sq+squares[i];
        if(p->board[s]!=EDGE && ((p->board[s])*color)<=0)
        {
            moves[c].fromSq=sq;
            moves[c].toSq=s;
            moves[c].piece=p->board[sq];
            moves[c].capture=p->board[s];
            c++;
        }
    }
    ac->moves=moves;
    ac->size=c;
    return ac;
}

array_Container* bishopMoves(Position *p,int sq)
{
    Move* moves=malloc(MAXBISHOPMOVES*sizeof(Move));
    array_Container *ac=malloc(sizeof(array_Container));
    if(ac==NULL)
    {
        printf("No memory for bishop moves\n");
        exit(1);
    }

    int color=p->board[sq]>0?1:-1;
    int squares[MAXBISHOPMOVES]; int count=0,i;
    int s=sq;
    while(1)
    {
        s-=9; int block=p->board[s]*color;
        if(p->board[s]==EDGE || block>0)
        {
            break;
        }
        squares[count]=s; count++;
        if(block<0)
        {
            break;
        }
    }
    s=sq;
    while(1)
    {
        s+=9; int block=p->board[s]*color;
        if(p->board[s]==EDGE || block>0)
        {
            break;
        }
 	    squares[count]=s; count++;
        if(block<0)
        {
            break;
        }
    }
    s=sq;
    while(1)
    {
        s-=11; int block=p->board[s]*color;
        if(p->board[s]==EDGE || block>0)
        {
            break;
        }
        squares[count]=s; count++;
        if(block<0)
        {
            break;
        }
    }
    s=sq;
    while(1)
    {
        s+=11; int block=p->board[s]*color;
        if(p->board[s]==EDGE || block>0)
        {
            break;
        }
        squares[count]=s; count++;
        if(block<0)
        {
            break;
        }
    }
    for(i=0;i<count;i++)
    {
        moves[i].fromSq=sq;
        moves[i].toSq=squares[i];
        moves[i].piece=p->board[sq];
        moves[i].capture=p->board[squares[i]];
    }
    ac->moves=moves;
    ac->size=count;
    return ac;
}

array_Container* rookMoves(Position *p,int sq)
{
    Move* moves=malloc(MAXROOKMOVES*sizeof(Move));
    array_Container *ac=malloc(sizeof(array_Container));
    if(ac==NULL)
    {
        printf("No memory for rook moves\n");
        exit(1);
    }

    int color=p->board[sq]>0?1:-1;
    int squares[MAXROOKMOVES]; int count=0,i;
    int s=sq;
    while(1)
    {
        s-=10; int block=p->board[s]*color;
        if(p->board[s]==EDGE || block>0)
        {
            break;
        }
        squares[count]=s; count++;
        if(block<0)
        {
            break;
        }
    }
    s=sq;
    while(1)
    {
        s+=10; int block=p->board[s]*color;
        if(p->board[s]==EDGE || block>0)
        {
            break;
        }
        squares[count]=s; count++;
        if(block<0)
        {
            break;
        }
    }
    s=sq;
    while(1)
    {
        s++; int block=p->board[s]*color;
        if(p->board[s]==EDGE || block>0)
        {
            break;
        }
        squares[count]=s; count++;
        if(block<0)
        {
            break;
        }
    }
    s=sq;
    while(1)
    {
        s--; int block=p->board[s]*color;
        if(p->board[s]==EDGE || block>0)
        {
            break;
        }
        squares[count]=s; count++;
        if(block<0)
        {
            break;
        }
    }
    for(i=0;i<count;i++)
    {
        moves[i].fromSq=sq;
        moves[i].toSq=squares[i];
        moves[i].piece=p->board[sq];
        moves[i].capture=p->board[squares[i]];
    }
    ac->moves=moves;
    ac->size=count;
    return ac;
}

array_Container* queenMoves(Position *p,int sq)
{
    Move* moves=malloc(MAXQUEENMOVES*sizeof(Move));
    array_Container *ac=malloc(sizeof(array_Container));
    if(ac==NULL)
    {
        printf("No memory for queen moves\n");
        exit(1);
    }

    array_Container *bishop=bishopMoves(p,sq);
    array_Container *rook=rookMoves(p,sq);
    int size=0,i;
    for(i=0;i<bishop->size;i++)
    {
        moves[size]=bishop->moves[i];
        moves[size].piece=p->board[sq];
        size++;
    }
    for(i=0;i<rook->size;i++)
    {
        moves[size]=rook->moves[i];
        moves[size].piece=p->board[sq];
        size++;
    }
    delete_Container(bishop);
    delete_Container(rook);
    ac->moves=moves;
    ac->size=size;
    return ac;
}

array_Container* dummyMoves(Position *p,int sq)
{
    Move* moves=malloc(MAXKINGMOVES*sizeof(Move));
    array_Container *ac=malloc(sizeof(array_Container));
    if(ac==NULL)
    {
        printf("No memory for dummy moves\n");
        exit(1);
    }

    int i,count=0;
    int squares[MAXKINGMOVES]={-11,-10,-9,-1,1,9,10,11};
    for(i=0;i<MAXKINGMOVES;i++)
    {
        int s=sq+squares[i];
        int samecol=p->board[s]*p->board[sq]<=0?1:-1;
        if(p->board[s]!=EDGE && samecol==1)
        {
            moves[count].toSq=s;
            moves[count].fromSq=sq;
            moves[count].piece=p->board[sq];
            moves[count].capture=p->board[s];
            count++;
        }
    }
    ac->moves=moves;
    ac->size=count;
    return ac;
}

void kingMoves(Position* p,int sq)
{
    return;
}

void display(Position *p)
{
    int i;
    for(i=0;i<SQUARES;i++)
    {
        switch(p->board[i])
        {
            case EDGE:       break;
            case EMPTY:      printf(". "); break;
            case WHITEPAWN:  printf("♙ "); break;
            case BLACKPAWN:  printf("♟ "); break;
            case WHITEKNIGHT:printf("♘ "); break;
            case BLACKKNIGHT:printf("♞ "); break;
            case WHITEBISHOP:printf("♗ "); break;
            case BLACKBISHOP:printf("♝ "); break;
            case WHITEROOK:  printf("♖ "); break;
            case BLACKROOK:  printf("♜ "); break;
            case WHITEQUEEN: printf("♕ "); break;
            case BLACKQUEEN: printf("♛ "); break;
            case WHITEKING:  printf("♔ "); break;
            case BLACKKING:  printf("♚ "); break;
            case MOVE:       printf("* "); break;
        }
        if((i+1)%10==0)
        {
            printf("\n");
        }
    }
    printf("\n");
}

Position* new_Position()
{
        Position *p=malloc(sizeof(Position));
        p->moveCount=0;
        setEdges(p);
        setBoard(p);

        return p;
}
