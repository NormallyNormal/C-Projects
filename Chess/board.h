#ifndef BOARD_HEADER
#define BOARD_HEADER

#include <stdint.h>
#include <stdbool.h>

struct Board {
    int64_t whitePawns;
    int64_t whiteKnights;
    int64_t whiteBishops;
    int64_t whiteRooks;
    int64_t whiteQueens;
    int64_t whiteKing;
    int64_t blackPawns;
    int64_t blackKnights;
    int64_t blackBishops;
    int64_t blackRooks;
    int64_t blackQueens;
    int64_t blackKing;
    int64_t enPassant;
    bool whiteKingKingsideCastle;
    bool whiteKingQueensideCastle;
    bool blackKingKingsideCastle;
    bool blackKingQueensideCastle;
};

#endif
