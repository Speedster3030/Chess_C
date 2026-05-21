#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "Position.h"

void getMoves(Position* p,moveList* moves)
{
    uint64_t pieces=p->turn?p->whitePieces:p->blackPieces;
    moves->count=0; int i;
    moveList allMoves; allMoves.count=0;

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
                pawnMoves(p,s,&allMoves);
                break;
            case WHITEKING:
                kingMoves(p,s,&allMoves);
                break;
            case WHITEKNIGHT:
                knightMoves(p,s,0,&allMoves);
                break;
            case WHITEBISHOP:
                bishopMoves(p,s,&allMoves);
                break;
            case WHITEROOK:
                rookMoves(p,s,&allMoves);
                break;
            case WHITEQUEEN:
                queenMoves(p,s,&allMoves);
                break;
            default:
                break;
        }
        pieces^=lowest;
    }

    //now account for checks and pins;

    for(i=0;i<allMoves.count;i++)
    {
        if(allMoves.list[i].capture>ENPASS)
        {
            goto label;
        }
        makeMove(p,&allMoves.list[i]);
        int8_t king=p->turn?p->blackKingSq:p->whiteKingSq;
        int kingAttacked= squAt(p,king,p->turn);
        unmakeMove(p);
        if(!kingAttacked)
        {
            label:
            moves->list[moves->count]=allMoves.list[i];
            moves->count++;
        }
    }
}

void makeMove(Position *p,Move *m)
{
    uint64_t* pieces=p->turn?&p->whitePieces:&p->blackPieces;
    uint64_t* oppPieces=p->turn?&p->blackPieces:&p->whitePieces;

    p->state[p->moveCount].whitePieces=p->whitePieces;
    p->state[p->moveCount].blackPieces=p->blackPieces;
    p->state[p->moveCount].whiteKing=p->whiteKing;
    p->state[p->moveCount].blackKing=p->blackKing;
    p->state[p->moveCount].whiteKingSq=p->whiteKingSq;
    p->state[p->moveCount].blackKingSq=p->blackKingSq;
    p->state[p->moveCount].flags=p->flags;
    memcpy(&p->state[p->moveCount].board[0],&p->board[0],SQUARES*sizeof(int8_t));

    if(m->capture==PROMO)
    {
        int t=BIT_SQ(m->toSq),f=BIT_SQ(m->fromSq);
        p->board[m->fromSq]=EMPTY;
        p->board[m->toSq]=m->piece;
        *pieces &= ~(1ULL << (63-f));
        *pieces |=1ULL << (63-t);
        *oppPieces &=~(1ULL << (63-t));
        goto label;
    }

    if(m->capture==ENPASS)
    {
        int i=m->piece>0?1:-1;
        int f=BIT_SQ(m->fromSq),t=BIT_SQ(m->toSq);
        int f0=BIT_SQ(m->toSq+(10*i));
        p->board[m->toSq]=m->piece;p->board[m->fromSq]=EMPTY;
        p->board[m->toSq+(10*i)]=EMPTY;

        *pieces &=~(1ULL << (63-f)); *pieces |=1ULL <<(63-t);
        *oppPieces &=~(1ULL << (63-f0));

        goto label;
    }

    if(m->capture > ENPASS)
    {
        uint64_t* king=p->turn?&p->whiteKing:&p->blackKing;
        int8_t* kingSq=p->turn?&p->whiteKingSq:&p->blackKingSq;
        *kingSq=m->toSq;
        int kt=BIT_SQ(m->toSq),kf=BIT_SQ(m->fromSq);
        *pieces &= ~(1ULL << (63-kf)); *pieces |= 1ULL << (63-kt);
        *king &= ~(1ULL << (63-kf));  *king |= 1ULL << (63-kt);
        p->board[m->toSq]=m->piece;   p->board[m->fromSq]=EMPTY;

        if(m->capture==KINGSIDECASTLE)
        {
            int rt=BIT_SQ(m->toSq-1),rf=BIT_SQ(m->toSq+1);
            p->board[m->toSq-1]=p->board[m->toSq+1];
            p->board[m->toSq+1]=EMPTY;
            *pieces &= ~(1ULL << (63-rf));
            *pieces |= 1ULL << (63-rt);
        }
        else
        {
            int rt=BIT_SQ(m->toSq+1), rf=BIT_SQ(m->toSq-2);
            p->board[m->toSq+1]=p->board[m->toSq-2];
            p->board[m->toSq-2]=EMPTY;
            *pieces &= ~(1ULL << (63-rf));
            *pieces |= 1ULL << (63-rt);
        }
        goto label;
    }

    int f=BIT_SQ(m->fromSq), t=BIT_SQ(m->toSq);
    //macro value directly in expressions gives errors

    *pieces &= ~(1ULL << (63-f));
    *pieces |= (1ULL << (63-t));
    if(m->capture!=0)
    {
        *oppPieces &= ~(1ULL << (63-t));
    }

    if(m->piece==WHITEKING)
    {
        p->whiteKing &= ~(1ULL << (63-f));
        p->whiteKing |= 1ULL << (63-t);
        p->whiteKingSq=m->toSq;
    }
    if(m->piece==BLACKKING)
    {
        p->blackKing &=~(1ULL << (63-f));
        p->blackKing |= 1ULL << (63-t);
        p->blackKingSq=m->toSq;
    }

    p->board[m->toSq]=p->board[m->fromSq];
    p->board[m->fromSq]=EMPTY;
    label:
    if((p->flags & 0b0001) && (m->toSq==98 || m->fromSq==98 || m->piece==WHITEKING))
    {
        p->flags &= 0b1110;
    }
    if((p->flags & 0b0010) && (m->toSq==91 || m->fromSq==91 || m->piece==WHITEKING))
    {
        p->flags &= 0b1101;
    }
    if((p->flags & 0b0100) && (m->toSq==28 || m->fromSq==28 || m->piece==BLACKKING))
    {
        p->flags &= 0b1011;
    }
    if((p->flags & 0b1000) && (m->toSq==21 || m->fromSq==21 || m->piece==BLACKKING))
    {
        p->flags &= 0b0111;
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

    p->whitePieces=p->state[p->moveCount-1].whitePieces;
    p->blackPieces=p->state[p->moveCount-1].blackPieces;
    p->whiteKing=p->state[p->moveCount-1].whiteKing;
    p->blackKing=p->state[p->moveCount-1].blackKing;
    p->whiteKingSq=p->state[p->moveCount-1].whiteKingSq;
    p->blackKingSq=p->state[p->moveCount-1].blackKingSq;
    p->flags=p->state[p->moveCount-1].flags;
    memcpy(&p->board[0],&p->state[p->moveCount-1].board[0],SQUARES*sizeof(int8_t));

    p->moveCount--;
    p->turn=!p->turn;
}


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
        //int flag=p->board[s]*p->board[sq];
        if(p->board[s]!=EDGE)
        {
            m[j].toSq=s;
            m[j].fromSq=sq;
            m[j].piece=p->board[sq];
            m[j].capture=p->board[s];
            (*c)++;j++;
        }
    }
}

void pawnMoves(Position* p,int sq,moveList* moves)
{
    int color=p->board[sq]>0?1:-1;
    int i,c=0,inc=(color==1)?-1:1;
    int wFlag=(color==1) && sq>80 && p->board[sq-10]==0 && p->board[sq-20]==0;
    int bFlag=(color!=1) && sq<39 && p->board[sq+10]==0 && p->board[sq+20]==0;
    Move m=p->movesMade[p->moveCount-1];
    int n=moves->count;

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

    if(moves->count>n && p->board[sq+(inc*20)]==EDGE)
    {
        for(i=n;i<moves->count;i++)
        {
            moves->list[i].capture=PROMO;
            moves->list[i].piece=(color*WHITEQUEEN);
            c++;
        }
        for(i=0;i<c;i++)
        {
            int j=0;
            while(j<3)
            {
                moves->list[moves->count].fromSq=sq;
                moves->list[moves->count].toSq=moves->list[n+i].toSq;
                moves->list[moves->count].piece=color*(WHITEKNIGHT+j);
                moves->list[moves->count].capture=PROMO;
                moves->count++; j++;
            }
        }
        return;
    }//have to handle cases carefully and correctly

    int e1=m.toSq==(sq+1) && abs(m.piece)==WHITEPAWN && (color*p->board[sq+1])<0 && abs(m.fromSq-m.toSq)==20;
    int e2=m.toSq==(sq-1) && abs(m.piece)==WHITEPAWN && (color*p->board[sq-1])<0 && abs(m.fromSq-m.toSq)==20;

    if(e1 || e2)
    {
        moves->list[moves->count].fromSq=sq;
        moves->list[moves->count].toSq=m.fromSq-(10*inc);
        moves->list[moves->count].piece=p->board[sq];
        moves->list[moves->count].capture=ENPASS;
        moves->count++;
    }

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

        if(flag && !squAt(p,s,!p->turn))
        {
            moves->list[moves->count].fromSq=sq;
            moves->list[moves->count].toSq=s;
            moves->list[moves->count].piece=p->board[sq];
            moves->list[moves->count].capture=p->board[s];
            moves->count++;
        }
    }

    if(p->turn && WHITECANCASTLEKINGSIDE && WHITEKINGCASTLEOK)
    {
        moves->list[moves->count].fromSq=sq;
        moves->list[moves->count].toSq=sq+2;
        moves->list[moves->count].piece=p->board[sq];
        moves->list[moves->count].capture=KINGSIDECASTLE;
        moves->count++;
    }
    if(p->turn && WHITECANCASTLEQUEENSIDE && WHITEQUEENCASTLEOK)
    {
        moves->list[moves->count].fromSq=sq;
        moves->list[moves->count].toSq=sq-2;
        moves->list[moves->count].piece=p->board[sq];
        moves->list[moves->count].capture=QUEENSIDECASTLE;
        moves->count++;
    }
    if(!p->turn && BLACKCANCASTLEKINGSIDE && BLACKKINGCASTLEOK)
    {
        moves->list[moves->count].fromSq=sq;
        moves->list[moves->count].toSq=sq+2;
        moves->list[moves->count].piece=p->board[sq];
        moves->list[moves->count].capture=KINGSIDECASTLE;
        moves->count++;
    }
    if(!p->turn && BLACKCANCASTLEQUEENSIDE && BLACKQUEENCASTLEOK)
    {
        moves->list[moves->count].fromSq=sq;
        moves->list[moves->count].toSq=sq-2;
        moves->list[moves->count].piece=p->board[sq];
        moves->list[moves->count].capture=QUEENSIDECASTLE;
        moves->count++;
    }
}

int squAt(Position* p, int sq, int attacker)
{
    /* attacker is the color of the attacking side,
    1 for white and -1 for black */

    attacker=(attacker==1)?1:-1;

    int knight=(attacker>0)?WHITEKNIGHT:BLACKKNIGHT;
    int queen=(attacker>0)?WHITEQUEEN:BLACKQUEEN;
    int rook=(attacker>0)?WHITEROOK:BLACKROOK;
    int bishop=(attacker>0)?WHITEBISHOP:BLACKBISHOP;
    int pawn=(attacker>0)?WHITEPAWN:BLACKPAWN;
    int king=(attacker>0)?WHITEKING:BLACKKING;

    //check for knight:

    int knDir[MAXKNIGHTMOVES]={-21,-19,-12,-8,8,12,19,21};
    int i;
    for(i=0;i<MAXKNIGHTMOVES;i++)
    {
        int s=sq+knDir[i];
        if(p->board[s]!=EDGE && p->board[s]==knight)
        {
            return 1;
        }
    }

    //check for bishop and diagonal Queen:

    int bDir[4]={-11,-9,9,11};
    for(i=0;i<4;i++)
    {
        int s=sq;
        while(1)
        {
            s+=bDir[i];
            int block=p->board[s]*attacker;
            if(p->board[s]==EDGE || block<0)
            {
                break;
            }
            if(block>0)
            {
                if(p->board[s]==bishop || p->board[s]==queen)
                {
                    return 1;
                }
                break;
            }
        }
    }

    //check for rook and straight queen:

    int rDir[4]={-10,-1,1,10};
    for(i=0;i<4;i++)
    {
        int s=sq;
        while(1)
        {
            s+=rDir[i];
            int block=p->board[s]*attacker;
            if(p->board[s]==EDGE || block<0)
            {
                break;
            }
            if(block>0)
            {
                if(p->board[s]==rook || p->board[s]==queen)
                {
                    return 1;
                }
                break;
            }
        }
    }

    //check for king:

    int kDir[8]={-11,-10,-9,-1,1,9,10,11};
    for(i=0;i<8;i++)
    {
        int s=sq+kDir[i];
        if(p->board[s]!=EDGE && p->board[s]==king)
        {
            return 1;
        }
    }

    //check for pawn:

    int pDir[2]={9,11};
    if(attacker<0)
    {
        pDir[0]*=-1;
        pDir[1]*=-1;
    }
    for(i=0;i<2;i++)
    {
        int s=sq+pDir[i];
        if(p->board[s]!=EDGE && p->board[s]==pawn)
        {
            return 1;
        }
    }

    //square not attacked by any enemy pieces:

    return 0;
}

Position* readFen(char* fen)
{
    int i=0,l=strlen(fen),sq=0;
    Position* p=malloc(sizeof(Position));
    setEdges(p);p->turn=0; p->flags=0b0;
    p->moveCount=0;

    while(sq<64)
    {
        char ch=fen[i];
        int s=BOARD_SQ(sq);
        if((int)ch<97 && (int)ch>65)
        {
            p->whitePieces |= 1ULL << (63-sq);
        }
        if((int)ch>97)
        {
            p->blackPieces |= 1ULL << (63-sq);
        }
        switch(ch)
        {
            case 'p':
                p->board[s]=-1;
                sq++; break;
            case 'r':
                p->board[s]=-5; sq++; break;
            case 'b':
                p->board[s]=-4; sq++; break;
            case 'n':
                p->board[s]=-3; sq++; break;
            case 'q':
                p->board[s]=-9; sq++; break;
            case 'k':
                p->blackKing |= 1ULL << (63-sq);
                p->board[s]=-10;
                p->blackKingSq=s;
                sq++; break;
            case 'P':
                p->board[s]=1;
                sq++; break;
            case 'R':
                p->board[s]=5; sq++; break;
            case 'B':
                p->board[s]=4; sq++; break;
            case 'N':
                p->board[s]=3; sq++; break;
            case 'Q':
                p->board[s]=9; sq++; break;
            case 'K':
                p->board[s]=10;
                p->whiteKingSq=s;
                p->whiteKing |= 1ULL << (63-sq);
                sq++; break;
            case '/':
                break;
            default:
                sq+=(ch-'0'); break;
        }
        i++;
    }
    while(i<l)
    {
        switch(fen[i])
        {
            case 'w':
                p->turn=1; break;
            case 'k':
                p->flags |= 0b0100; break;
            case 'q':
                p->flags |= 0b1000; break;
            case 'Q':
                p->flags |= 0b0010; break;
            case 'K':
                p->flags |= 0b0001; break;
            default:
                break;
        }
        i++;
    }
    return p;
}

void display(int8_t *p)
{
    int i;
    for(i=0;i<SQUARES;i++)
    {
        switch(p[i])
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
        p->moveCount=0;p->flags=0b00001111;
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
    p->whitePieces=0x000000000000ffff;
    p->blackPieces=0xffff000000000000;
    p->whiteKing=  1ULL << 3;
    p->whiteKingSq=95;
    p->blackKing=  1ULL << (63-4);
    p->blackKingSq=25;
    p->turn=1;
}

