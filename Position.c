#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "Position.h"

int inCheck(Position* p)
{
    uint64_t king=p->turn?p->whiteKing:p->blackKing;
    return king & p->attackMap;
}

void getMoves(Position* p,moveList* moves)
{
    uint64_t pieces=p->turn?p->whitePieces:p->blackPieces;
    moves->count=0; int i;

    while(pieces)
    {
        uint64_t lowest= pieces & -pieces;
        int bit=0;
        uint64_t temp= lowest;
        while(temp <<=1) bit++;
        int s=BOARD_SQ(bit);
        int piece=p->board[s];

        switch(abs(piece))
        {
            case WHITEPAWN:
                pawnMoves(p,s,moves);
                break;
            case WHITEKING:
                kingMoves(p,s,moves);
                break;
            case WHITEKNIGHT:
                knightMoves(p,s,0,moves);
                break;
            case WHITEBISHOP:
                bishopMoves(p,s,moves);
                break;
            case WHITEROOK:
                rookMoves(p,s,moves);
                break;
            case WHITEQUEEN:
                queenMoves(p,s,moves);
                break;
            default:
                break;
        }
        pieces^=lowest;
    }
}

/*How to make the makeMove function more efficinent?
  how to make it more fast?*/
void makeMove(Position *p,Move *m)
{
    int f=BIT_SQ(m->fromSq), t=BIT_SQ(m->toSq);
    //macro value directly in expressions gives errors

    p->state[p->moveCount].whitePawns=p->whitePawns;
    p->state[p->moveCount].blackPawns=p->blackPawns;
    p->state[p->moveCount].whitePieces=p->whitePieces;
    p->state[p->moveCount].blackPieces=p->blackPieces;
    p->state[p->moveCount].attackMap=p->attackMap;
    p->state[p->moveCount].whiteKing=p->whiteKing;
    p->state[p->moveCount].blackKing=p->blackKing;

    uint64_t pieces=p->turn?p->whitePieces:p->blackPieces;
    uint64_t oppPieces=p->turn?p->blackPieces:p->whitePieces;

    pieces &= ~(1ULL << (63-f));
    pieces |= (1ULL << (63-t));
    if(m->capture!=0)
    {
        oppPieces &= ~(1ULL << (63-t));
    }

    p->whitePieces=p->turn?pieces:oppPieces;
    p->blackPieces=p->turn?oppPieces:pieces;

    if(m->piece==WHITEPAWN)
    {
        p->whitePawns &= ~(1ULL << (63-f));
        p->whitePawns |= 1ULL << (63-t);
    }
    if(m->capture==BLACKPAWN)
    {
        p->blackPawns &= ~(1ULL << (63-t));
    }
    if(m->piece==BLACKPAWN)
    {
        p->blackPawns &= ~(1ULL << (63-f));
        p->blackPawns |= 1ULL << (63-t);
    }
    if(m->capture==WHITEPAWN)
    {
        p->whitePawns &= ~(1ULL << (63-t));
    }
    if(m->piece==WHITEKING)
    {
        p->whiteKing &= ~(1ULL << (63-f));
        p->whiteKing |= 1ULL << (63-t);
    }
    if(m->piece==BLACKKING)
    {
        p->blackKing &=~(1ULL << (63-f));
        p->blackKing |= 1ULL << (63-t);
    }

    p->movesMade[p->moveCount]=*m;
    p->moveCount++;
    p->turn=!p->turn;
    p->board[m->toSq]=p->board[m->fromSq];
    p->board[m->fromSq]=EMPTY;
    generateAttackMap(p);
}

void unmakeMove(Position *p)
{
    if(p->moveCount==0)
    {
        printf("ERROR! NO MOVES TO UNDO\n");
        return;
    }
    Move m=p->movesMade[p->moveCount-1];

    p->whitePawns=p->state[p->moveCount-1].whitePawns;
    p->blackPawns=p->state[p->moveCount-1].blackPawns;
    p->whitePieces=p->state[p->moveCount-1].whitePieces;
    p->blackPieces=p->state[p->moveCount-1].blackPieces;
    p->attackMap=p->state[p->moveCount-1].attackMap;
    p->whiteKing=p->state[p->moveCount-1].whiteKing;
    p->blackKing=p->state[p->moveCount-1].blackKing;

    p->moveCount--;
    p->turn=!p->turn;
    p->board[m.toSq]=m.capture;
    p->board[m.fromSq]=m.piece;
}
/* 21 Dec 2025; how do we go about detecting pinned pieces? we can try making each 
move and checking if our king is in check, that is the most intuitive way, but
it will cost time and performance,,, is there a way to detect pins without actually
moving pieces?? a method that saves time?? Bitboard manipulation, etc? */
void knightMoves(Position *p,int sq,int attack,moveList* moves)
{
    int i,color=p->board[sq]>0?1:-1;
    int squares[MAXKNIGHTMOVES]={-21,-19,-12,-8,8,12,19,21};
    Move* m=&moves->list[moves->count];
    int* c=&moves->count;
    int j=0;
    for(i=0;i<MAXKNIGHTMOVES;i++)
    {
        int s=sq+squares[i];
        int flag=attack?-1:p->board[s]*color;
        if(p->board[s]!=EDGE && flag<=0)
        {
            m[j].fromSq=sq;
            m[j].toSq=s;
            m[j].piece=p->board[sq];
            m[j].capture=p->board[s];
            (*c)++;j++;
        }
    }
}

void slideMoves(Position* p,int sq,int color,int dir,int* squares,int* count)
{
    int s=sq;
    while(1)
    {
        s+=dir;
        int block=p->board[s]*color;
        if(p->board[s]==EDGE || block>0)
        {
            break;
        }
        squares[*count]=s;(*count)++;
        if(block<0)
        {
            break;
        }
   }
}

void bishopMoves(Position *p,int sq,moveList* moves)
{
    int color=p->board[sq]>0?1:-1;
    int squares[MAXBISHOPMOVES]; int i,count=0;
    Move* m=&moves->list[moves->count];
    int* c=&moves->count;
    int dir[4]={-9,9,-11,11};

    for(i=0;i<4;i++)
    {
        slideMoves(p,sq,color,dir[i],&squares[0],&count);
    }

    for(i=0;i<count;i++)
    {
        m[i].fromSq=sq;
        m[i].toSq=squares[i];
        m[i].piece=p->board[sq];
        m[i].capture=p->board[squares[i]];
        (*c)++;
    }
}

void rookMoves(Position* p,int sq,moveList* moves)
{
    int color=p->board[sq]>0?1:-1;
    int squares[MAXROOKMOVES]; int i,count=0;
    Move* m=&moves->list[moves->count];
    int* c=&moves->count;
    int dir[4]={-10,-1,1,10};

    for(i=0;i<4;i++)
    {
        slideMoves(p,sq,color,dir[i],&squares[0],&count);
    }

    for(i=0;i<count;i++)
    {
        m[i].fromSq=sq;
        m[i].toSq=squares[i];
        m[i].piece=p->board[sq];
        m[i].capture=p->board[squares[i]];
        (*c)++;
    }
}

void queenMoves(Position *p,int sq,moveList* moves)
{
    int color=p->board[sq]>0?1:-1;
    int squares[MAXQUEENMOVES]; int i,count=0;
    Move* m=&moves->list[moves->count];
    int* c=&moves->count;
    int dir[8]={-11,-10,-9,-1,1,9,10,11};

    for(i=0;i<8;i++)
    {
        slideMoves(p,sq,color,dir[i],&squares[0],&count);
    }

    for(i=0;i<count;i++)
    {
        m[i].fromSq=sq;
        m[i].toSq=squares[i];
        m[i].piece=p->board[sq];
        m[i].capture=p->board[squares[i]];
        (*c)++;
    }
}

void dummyMoves(Position *p,int sq,moveList* moves)
{
    int i,j=0;
    int squares[MAXKINGMOVES]={-11,-10,-9,-1,1,9,10,11};
    Move* m=&moves->list[moves->count];
    int* c=&moves->count;
    for(i=0;i<MAXKINGMOVES;i++)
    {
        int s=sq+squares[i];
        int flag=p->board[s]*p->board[sq];
        if(p->board[s]!=EDGE && flag<=0)
        {
            m[j].toSq=s;
            m[j].fromSq=sq;
            m[j].piece=p->board[sq];
            m[j].capture=p->board[s];
            (*c)++;j++;
        }
    }
}

uint64_t pawnAttacks(Position* p,int color)
{
    uint64_t d=0;
    uint64_t pawns=color?p->whitePawns:p->blackPawns;
    int i=color?-1:1;

    while(pawns)
    {
        uint64_t lowest= pawns & -pawns;
        int bit=0;
        uint64_t temp= lowest;
        while(temp <<=1) bit++;

        int sq=BOARD_SQ(bit);
        int l=sq+(i*9),r=sq+(i*11);
        if(p->board[l]!=EDGE)
        {
            int bitL=BIT_SQ(l);
            d |= 1ULL<<(63-bitL);
        }
        if(p->board[r]!=EDGE)
        {
            int bitR=BIT_SQ(r);
            d |= 1ULL<<(63-bitR);
        }

        pawns^=lowest;
    }

    return d;
}

void pawnMoves(Position* p,int sq,moveList* moves)
{
    int color=p->board[sq]>0?1:-1;
    int i;
    int inc=(color==1)?-1:1;
    int wFlag=(color==1) && sq>80 && p->board[sq-10]==0 && p->board[sq-20]==0;
    int bFlag=(color!=1) && sq<39 && p->board[sq+10]==0 && p->board[sq+20]==0;

    if(p->board[sq+(inc*10)]==0)
    {
        moves->list[moves->count].fromSq=sq;
        moves->list[moves->count].toSq=sq+(inc*10);
        moves->list[moves->count].piece=p->board[sq];
        moves->list[moves->count].capture=0;
        moves->count++;//one push
    }

    if(wFlag || bFlag)
    {
        moves->list[moves->count].fromSq=sq;
        moves->list[moves->count].toSq=sq+(inc*20);
        moves->list[moves->count].piece=p->board[sq];
        moves->list[moves->count].capture=0;
        moves->count++;//double push
    }

    int c1=p->board[sq+(inc*9)];
    int c2=p->board[sq+(inc*11)];

    if(c1!=EDGE && c1*color<0)
    {
        moves->list[moves->count].fromSq=sq;
        moves->list[moves->count].toSq=sq+(inc*9);
        moves->list[moves->count].piece=p->board[sq];
        moves->list[moves->count].capture=c1;
        moves->count++;
    }

    if(c2!=EDGE && c2*color<0)
    {
        moves->list[moves->count].fromSq=sq;
        moves->list[moves->count].toSq=sq+(inc*11);
        moves->list[moves->count].piece=p->board[sq];
        moves->list[moves->count].capture=c2;
        moves->count++;
    }

    Move m=p->movesMade[p->moveCount-1];
    //int enPass=(abs(m.piece)==WHITEPAWN)&&(abs(m.toSq-m.fromSq)==20)&&(abs(p-m.toSq)==1)?1:0;

    /*if(enPass)
    {
        p->moves.list[p->moves.count].fromSq=sq;
        p->moves.list[p->moves.count].toSq=m.fromSq-inc;
        p->moves.list[p->moves.count].piece=p->board[sq];
        p->moves.list[p->moves.count].capture=m.piece;
    }//how to make and undo en Passant?*/

    /*now we need enpassant, castling,
    promotion, pinned pieces...
    Pinned Pieces can only be of the king's own
    color, and can only be pinned by sliding
    pieces, so we can take a queens moves from
    the kings square, and check if there is
    only one same color piece between an enemy
    sliding piece and our king;*/
}

uint64_t bitMoves(Position* p,int sq,int dir,int color,uint64_t moves)
{
    int bit=BIT_SQ(sq);
    uint64_t friends=color==1?p->whitePieces:p->blackPieces;
    if(p->board[sq]==EDGE)
    {
        return moves;
    }
    uint64_t enemies=color==1?p->blackPieces:p->whitePieces;
    moves |=(1ULL<<(63-bit));
    if(p->board[sq]!=EMPTY)
    {
        return moves;
    }
    return bitMoves(p,sq+dir,dir,color,moves);
}

void generateAttackMap(Position* p)
{
    uint64_t pieces=(p->turn==1)?p->blackPieces:p->whitePieces;
    p->attackMap=0;
    moveList attackMoves;
    attackMoves.count=0;
    int i;
    int color=p->turn==1?-1:1;

    while(pieces)
    {
        uint64_t lowest= pieces & -pieces;
        int bit=0;
        uint64_t temp= lowest;
        while(temp <<= 1) bit++;

        int sq=BOARD_SQ(bit);
        int piece=p->board[sq];
        uint64_t moves=0ULL;

        switch(abs(piece))
        {
            case WHITEBISHOP:
                moves |=bitMoves(p,sq-11,-11,color,0ULL);
                moves |=bitMoves(p,sq-9,-9,color,0ULL);
                moves |=bitMoves(p,sq+9,9,color,0ULL);
                moves |=bitMoves(p,sq+11,11,color,0ULL);
                break;
            case WHITEKNIGHT:
                knightMoves(p,sq,1,&attackMoves);
                break;
            case WHITEROOK:
                moves |=bitMoves(p,sq-10,-10,color,0ULL);
                moves |=bitMoves(p,sq-1,-1,color,0ULL);
                moves |=bitMoves(p,sq+1,1,color,0ULL);
                moves |=bitMoves(p,sq+10,10,color,0ULL);
                break;
            case WHITEQUEEN:
                moves |=bitMoves(p,sq-11,-11,color,0ULL);
                moves |=bitMoves(p,sq-9,-9,color,0ULL);
                moves |=bitMoves(p,sq+9,9,color,0ULL);
                moves |=bitMoves(p,sq+11,11,color,0ULL);
                moves |=bitMoves(p,sq-10,-10,color,0ULL);
                moves |=bitMoves(p,sq-1,-1,color,0ULL);
                moves |=bitMoves(p,sq+1,1,color,0ULL);
                moves |=bitMoves(p,sq+10,10,color,0ULL);
                break;
            case WHITEKING:
                dummyMoves(p,sq,&attackMoves);
                break;
            default:
                break;
        }

        pieces^=lowest;
        p->attackMap |=moves;
    }

    for(i=0;i<attackMoves.count;i++)
    {
        int sq=attackMoves.list[i].toSq;
        int bitSq=BIT_SQ(sq);
        p->attackMap |=(1ULL << (63-bitSq));
    }
    p->attackMap |=pawnAttacks(p,!p->turn);
}

void kingMoves(Position* p,int sq,moveList* moves)
{
    int i;
    int squares[MAXKINGMOVES]={-11,-10,-9,-1,1,9,10,11};

    for(i=0;i<MAXKINGMOVES;i++)
    {
        int s=sq+squares[i];
        int bit=BIT_SQ(s);
        int flag=(p->board[s]!=EDGE && p->board[sq]*p->board[s]<=0)?1:0;

        if(flag && !(p->attackMap & (1ULL << (63-bit))))
        {
            moves->list[moves->count].fromSq=sq;
            moves->list[moves->count].toSq=s;
            moves->list[moves->count].piece=p->board[sq];
            moves->list[moves->count].capture=p->board[s];
            moves->count++;
        }
    }
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
            case WHITEPAWN:  printf(WHITE "♟ " RESET); break;
            case BLACKPAWN:  printf(BROWN "♟ " RESET); break;
            case WHITEKNIGHT:printf(WHITE "♞ " RESET); break;
            case BLACKKNIGHT:printf(BROWN "♞ " RESET); break;
            case WHITEBISHOP:printf(WHITE "♝ " RESET); break;
            case BLACKBISHOP:printf(BROWN "♝ " RESET); break;
            case WHITEROOK:  printf(WHITE "♜ " RESET); break;
            case BLACKROOK:  printf(BROWN "♜ " RESET); break;
            case WHITEQUEEN: printf(WHITE "♛ " RESET); break;
            case BLACKQUEEN: printf(BROWN "♛ " RESET); break;
            case WHITEKING:  printf(WHITE "♚ " RESET); break;
            case BLACKKING:  printf(BROWN "♚ " RESET); break;
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
        //p->moves.count=0;
        //p->attackMoves.count=0;
        setEdges(p);
        setBoard(p);

        return p;
}

void displayBits(uint64_t b)
{
    int i;
    for(i=0;i<64;i++)
    {
        if(b & (1ULL<<(63-i)))
        {
            printf("* ");
        }
        else
        {
            printf(". ");
        }
        if(i%8==7)
        {
            printf("\n");
        }
    }
    printf("\n");
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
    p->whitePawns= 0x000000000000ff00;
    p->blackPawns= 0x00ff000000000000;
    p->whiteKing=  1ULL << 3;
    p->blackKing=  1ULL << (63-4);
    p->turn=1;
    generateAttackMap(p);
}
