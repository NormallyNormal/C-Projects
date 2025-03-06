#ifndef MOVES_HEADER
#define MOVES_HEADER

#include <stdint.h>
#include "board.h"

enum moveInfo {
    VAL_RANK_1 = 0,
    VAL_FILE_A = 0,
    VAL_RANK_2 = 1,
    VAL_FILE_B = 1,
    VAL_RANK_3 = 2,
    VAL_FILE_C = 2,
    VAL_RANK_4 = 3,
    VAL_FILE_D = 3,
    VAL_RANK_5 = 4,
    VAL_FILE_E = 4,
    VAL_RANK_6 = 5,
    VAL_FILE_F = 5,
    VAL_RANK_7 = 6,
    VAL_FILE_G = 6,
    VAL_RANK_8 = 7,
    VAL_FILE_H = 7,

    WHITE = 8,
    BLACK = 10,
    PAWN_PROMOTION = 11,
    EN_PASSANT = 12,

    WHITE_PAWN = 13,
    WHITE_KNIGHT = 14,
    WHITE_BISHOP = 15,
    WHITE_ROOK = 16,
    WHITE_QUEEN = 17,
    WHITE_KING = 18,

    BLACK_PAWN = 19,
    BLACK_KNIGHT = 20,
    BLACK_BISHOP = 21,
    BLACK_ROOK = 22,
    BLACK_QUEEN = 23,
    BLACK_KING = 24
};

struct Move {
    uint16_t startRank : 3;
    uint16_t startFile : 3;
    uint16_t endRank : 5;
    uint16_t endFile : 5;
};

struct MoveList {
    struct Move moves[256];
    int8_t size;
};

static const int64_t RANK_1 = -72057594037927936LL;
static const int64_t RANK_4 = 1095216660480LL;
static const int64_t RANK_5 = 4278190080LL;
static const int64_t RANK_8 = 255LL;

static const int64_t FILE_A = 72340172838076673LL;
static const int64_t FILE_H = -9187201950435737472LL;
static const int64_t FILE_AB = 217020518514230019LL;
static const int64_t FILE_GH = -4557430888798830400LL;

static const int64_t KNIGHT_SPAN = 43234889994LL;
static const int64_t KING_SPAN = 460039LL;

static const int64_t RANK_MASKS[] =
        {
                0xFFLL, 0xFF00LL, 0xFF0000LL, 0xFF000000LL, 0xFF00000000LL, 0xFF0000000000LL, 0xFF000000000000LL, 0xFF00000000000000LL
        };
static const int64_t FILE_MASKS[] =
        {
                0x101010101010101LL, 0x202020202020202LL, 0x404040404040404LL, 0x808080808080808LL,
                0x1010101010101010LL, 0x2020202020202020LL, 0x4040404040404040LL, 0x8080808080808080LL
        };
static const int64_t DIAGONAL_MASKS[] =
        {
                0x1LL, 0x102LL, 0x10204LL, 0x1020408LL, 0x102040810LL, 0x10204081020LL, 0x1020408102040LL,
                0x102040810204080LL, 0x204081020408000LL, 0x408102040800000LL, 0x810204080000000LL,
                0x1020408000000000LL, 0x2040800000000000LL, 0x4080000000000000LL, 0x8000000000000000LL
        };
static const int64_t ANTI_DIAGONAL_MASKS[] =
        {
                0x80LL, 0x8040LL, 0x804020LL, 0x80402010LL, 0x8040201008LL, 0x804020100804LL, 0x80402010080402LL,
                0x8040201008040201LL, 0x4020100804020100LL, 0x2010080402010000LL, 0x1008040201000000LL,
                0x804020100000000LL, 0x402010000000000LL, 0x201000000000000LL, 0x100000000000000LL
        };

static const int64_t CASTLE_ROOKS[]={63,56,7,0};

void applyMove(struct Board * board, struct Board * nextBoard, struct Move theMove);
int64_t move(int64_t board, struct Move move, enum moveInfo type);
int64_t makeMoveEnPassant(int64_t board, struct Move move);
int64_t makeMoveCastle(int64_t rookBoard, int64_t kingBoard, struct Move move, enum moveInfo type);
void possibleMovesWhite(struct MoveList * moves, struct Board * board);
void possibleMovesBlack(struct MoveList * moves, struct Board * board);
void possiblePawnMovesWhite(struct MoveList * moves, int64_t wp, int64_t bp, int64_t ep);
void possiblePawnMovesBlack(struct MoveList * moves, int64_t wp, int64_t bp, int64_t ep);
void possibleKnightMoves(struct MoveList * moves, int64_t wn);
void possibleBishopMoves(struct MoveList * moves, int64_t wb);
void possibleRookMoves(struct MoveList * moves, int64_t wr);
void possibleQueenMoves(struct MoveList * moves, int64_t wq);
void possibleKingMovesWhite(struct MoveList * moves, int64_t wp,int64_t wn,int64_t wb,int64_t wr,int64_t wq,int64_t wk,int64_t bp,int64_t bn,int64_t bb,int64_t br,int64_t bq,int64_t bk);
void possibleKingMovesBlack(struct MoveList * moves, int64_t wp,int64_t wn,int64_t wb,int64_t wr,int64_t wq,int64_t wk,int64_t bp,int64_t bn,int64_t bb,int64_t br,int64_t bq,int64_t bk);
int64_t whiteAttacks(int64_t wp,int64_t wn,int64_t wb,int64_t wr,int64_t wq,int64_t wk,int64_t bp,int64_t bn,int64_t bb,int64_t br,int64_t bq,int64_t bk);
int64_t blackAttacks(int64_t wp,int64_t wn,int64_t wb,int64_t wr,int64_t wq,int64_t wk,int64_t bp,int64_t bn,int64_t bb,int64_t br,int64_t bq,int64_t bk);
void possibleCastleWhite(struct MoveList * moves, int64_t wp, int64_t wn, int64_t wb, int64_t wr, int64_t wq, int64_t wk, int64_t bp, int64_t bn, int64_t bb, int64_t br, int64_t bq, int64_t bk, bool wck, bool wcq);
void possibleCastleBlack(struct MoveList * moves, int64_t wp, int64_t wn, int64_t wb, int64_t wr, int64_t wq, int64_t wk, int64_t bp, int64_t bn, int64_t bb, int64_t br, int64_t bq, int64_t bk, bool wck, bool wcq);
int64_t straightMoves(int location);
int64_t diagonalMoves(int location);
bool checkCheckmate(struct Board * board, bool WhiteToMove);
void addMove(struct MoveList * moves, enum moveInfo startRank, enum moveInfo startFile, enum moveInfo endRank, enum moveInfo endFile);
int64_t reverse(int64_t bits);

#endif
