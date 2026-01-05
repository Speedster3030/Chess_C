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

#define MAXGAMEMOVES 150
#define MAXKNIGHTMOVES 8
#define MAXBISHOPMOVES 13
#define MAXROOKMOVES 14
#define MAXQUEENMOVES 27
#define MAXKINGMOVES 8
#define MOVE -11
#define MAXMOVES 137

#define BIT_SQ(n) n-17-(2*(n/10))
#define BOARD_SQ(n) n+21+(2*(n/8))

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
}gameState;

typedef struct
{
    int8_t board[SQUARES];
    Move movesMade[MAXGAMEMOVES];
    uint8_t moveCount;
    uint8_t turn;
    uint8_t flags;/*first bit from the right is for the white king side castle,
    second bit is for the white queen side castle, third for the black king side
    castle, and fourth for the black queen side castle. Or maybe we could do it 
    individually ? Like if the white king has moved or not in one bit, the black
    king in another bit, all of the rooks in another bit .. this way it seems 
    we will have to do a lot less if else checks. And attacked squares or 
    occupied squares can be checked by the bitboards... */
    uint64_t whitePieces;
    uint64_t blackPieces;
    uint64_t whitePawns;
    uint64_t blackPawns;
    uint64_t attackMap;
    uint64_t whiteKing;
    uint64_t blackKing;
    gameState state[MAXGAMEMOVES];
}Position;

/*typedef struct
{
    Move *moves;
    uint8_t size;
}array_Container;*/


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
void display(Position *p);
Position* new_Position();
void generateAttackMap(Position* p);
void displayBits(uint64_t b);




#endif
