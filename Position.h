#ifndef POSITION_H

#define POSITION_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define SQUARES 120

#define WHITEPAWN 1
#define BLACKPAWN -1
#define WHITEKNIGHT 3
#define BLACKKNIGHT -3
#define WHITEROOK 5
#define BLACKROOK -5
#define WHITEBISHOP 4
#define BLACKBISHOP -4
#define WHITEQUEEN 9
#define BLACKQUEEN -9
#define WHITEKING 10
#define BLACKKING -10

#define EMPTY 0
#define EDGE 11
#define PROMO 12
#define ENPASS 13
#define KINGSIDECASTLE 14
#define QUEENSIDECASTLE 15

#define MAXGAMEMOVES 150
#define MAXKNIGHTMOVES 8
#define MAXBISHOPMOVES 13
#define MAXROOKMOVES 14
#define MAXQUEENMOVES 27
#define MAXKINGMOVES 8
#define MOVE -11
#define MAXMOVES 137

#define BIT_SQ(n) (n)-17-(2*((n)/10))
#define BOARD_SQ(n) (n)+21+(2*((n)/8))

#define WHITECANCASTLEKINGSIDE (p->flags & 0b0001)
#define WHITECANCASTLEQUEENSIDE (p->flags & 0b010)
#define BLACKCANCASTLEKINGSIDE (p->flags & 0b0100)
#define BLACKCANCASTLEQUEENSIDE (p->flags & 0b1000)

#define WKPIECES ((p->whitePieces | p->blackPieces) & 0x6ULL)
#define WQPIECES ((p->whitePieces | p->blackPieces) & 0x70ULL)
#define BKPIECES ((p->whitePieces | p->blackPieces) & (0x6ULL << 56))
#define BQPIECES ((p->whitePieces | p->blackPieces) & (0x70ULL << 56))

#define WHITEKINGCASTLEOK (WKPIECES | (p->attackMap & 0xeULL))
#define WHITEQUEENCASTLEOK (WQPIECES | (p->attackMap & 0x38ULL))
#define BLACKKINGCASTLEOK (BKPIECES | (p->attackMap & (0xeULL << 56)))
#define BLACKQUEENCASTLEOK (BQPIECES | (p->attackMap & (0x38ULL << 56)))

#define RESET "\033[0m"
#define WHITE "\033[33m"
#define BROWN "\033[35m"

typedef struct
{
    int8_t fromSq;
    int8_t toSq;
    int8_t piece;
    int8_t capture;
}Move;

typedef struct
{
    Move list[MAXMOVES];
    int count;
}moveList;

typedef struct
{
    uint64_t attackMap;
    uint64_t whitePieces;
    uint64_t blackPieces;
    uint64_t whitePawns;
    uint64_t blackPawns;
    uint64_t whiteKing;
    uint64_t blackKing;
    uint8_t flags;
    int8_t board[SQUARES];
}gameState;

typedef struct
{
    int8_t board[SQUARES];
    Move movesMade[MAXGAMEMOVES];
    uint8_t moveCount;
    uint8_t turn;
    uint8_t flags;
    uint64_t whitePieces;
    uint64_t blackPieces;
    uint64_t whitePawns;
    uint64_t blackPawns;
    uint64_t attackMap;
    uint64_t whiteKing;
    uint64_t blackKing;
    gameState state[MAXGAMEMOVES];
}Position;

int inCheck(Position* p);
uint64_t bitMoves(Position* p,int s,int d,int c,uint64_t m);
void setEdges(Position *p);
void setBoard(Position *p);
void getMoves(Position* p,moveList* moves);
void makeMove(Position *p,Move *m);
void unmakeMove(Position *p);
void knightMoves(Position *p,int s,int a,moveList* moves);
void slideMoves(Position* p,int s,int c,int d,int* arr,int* co);
void bishopMoves(Position *p,int s,moveList* moves);
void rookMoves(Position *p,int s,moveList* moves);
void queenMoves(Position *p,int s,moveList* moves);
void dummyMoves(Position *p,int s,moveList* moves);
void pawnMoves(Position* p,int s,moveList* moves);
uint64_t pawnAttacks(Position* p,int c);
void kingMoves(Position *p,int s,moveList* moves);
void display(Position* p);
Position* new_Position();
Position* readFen(char* s);
uint64_t generateAttackMap(Position* p);
void displayBits(uint64_t b);




#endif
