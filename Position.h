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
#define MAXGAMEMOVES 200
#define MAXKNIGHTMOVES 8
#define MAXBISHOPMOVES 13
#define MAXROOKMOVES 14
#define MAXQUEENMOVES 27
#define MAXKINGMOVES 8
#define MOVE -11
#define MAXMOVES 137
#define BIT_SQ(n) n-17-(2*(n/10))
#define BOARD_SQ(n) n+21+(2*(n/8))

typedef struct
{
    int8_t fromSq;
    int8_t toSq;
    int8_t piece;
    int8_t capture;
} Move;

typedef struct
{
    int8_t board[SQUARES];
    Move movesMade[MAXGAMEMOVES];
    uint8_t moveCount;
    int8_t turn;
    unsigned long long whitePieces;
    unsigned long long blackPieces;
    unsigned long long whitePawns;
    unsigned long long blackPawns;
    unsigned long long attackMap;
} Position;

typedef struct
{
    Move *moves;
    uint8_t size;
}array_Container;


void delete_Container(array_Container *ac);
void setEdges(Position *p);
void setBoard(Position *p);
void makeMove(Position *p,Move *m);
void unmakeMove(Position *p);
array_Container* knightMoves(Position *p,int s);
array_Container* bishopMoves(Position *p,int s);
array_Container* rookMoves(Position *p,int s);
array_Container* queenMoves(Position *p,int s);
array_Container* dummyMoves(Position *p,int s);
void kingMoves(Position *p,int s);
void display(Position *p);
Position* new_Position();




#endif
