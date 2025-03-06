#include <stdint.h>

#include <stdlib.h>

#include <stdio.h>

#include <stdbool.h>

#include "moves.h"

static int64_t WHITE_PIECES;
static int64_t BLACK_PIECES;

static int64_t EMPTY;
static int64_t OCCUPIED;

static int64_t CAN_CAPTURE;

void applyMove(struct Board * board, struct Board * nextBoard, struct Move theMove) {
    nextBoard -> whitePawns = move(board -> whitePawns, theMove, WHITE_PAWN);
    nextBoard -> whiteKnights = move(board -> whiteKnights, theMove, WHITE_KNIGHT);
    nextBoard -> whiteBishops = move(board -> whiteBishops, theMove, WHITE_BISHOP);
    nextBoard -> whiteRooks = move(board -> whiteRooks, theMove, WHITE_ROOK);
    nextBoard -> whiteQueens = move(board -> whiteQueens, theMove, WHITE_QUEEN);
    nextBoard -> whiteKing = move(board -> whiteKing, theMove, WHITE_KING);

    nextBoard -> blackPawns = move(board -> blackPawns, theMove, BLACK_PAWN);
    nextBoard -> blackKnights = move(board -> blackKnights, theMove, BLACK_KNIGHT);
    nextBoard -> blackBishops = move(board -> blackBishops, theMove, BLACK_BISHOP);
    nextBoard -> blackRooks = move(board -> blackRooks, theMove, BLACK_ROOK);
    nextBoard -> blackQueens = move(board -> blackQueens, theMove, BLACK_QUEEN);
    nextBoard -> blackKing = move(board -> blackKing, theMove, BLACK_KING);
    
    nextBoard -> enPassant = move(board -> whitePawns | board -> blackPawns, theMove, BLACK_KING);

    nextBoard -> whiteRooks = makeMoveCastle(nextBoard -> whiteRooks, board -> whiteKing, theMove, WHITE_ROOK);
    nextBoard -> blackRooks = makeMoveCastle(nextBoard -> blackRooks, board -> blackKing, theMove, BLACK_ROOK);

    nextBoard -> whiteKingKingsideCastle = board -> whiteKingKingsideCastle;
    nextBoard -> whiteKingQueensideCastle = board -> whiteKingQueensideCastle;
    nextBoard -> blackKingKingsideCastle = board -> blackKingKingsideCastle;
    nextBoard -> blackKingQueensideCastle = board -> blackKingQueensideCastle;

    if (theMove.endFile <= VAL_FILE_H) { //'regular' move
        int start = theMove.startRank * 8 + theMove.startFile;

        if (((1LL << start) & board -> whiteKing) != 0) {
            nextBoard -> whiteKingKingsideCastle = false;
            nextBoard -> whiteKingQueensideCastle = false;
        } else if (((1LL << start) & board -> blackKing) != 0) {
            nextBoard -> blackKingKingsideCastle = false;
            nextBoard -> blackKingQueensideCastle = false;
        } else if (((1LL << start) & board -> whiteRooks & (1LL << CASTLE_ROOKS[0])) != 0) {
            nextBoard -> whiteKingKingsideCastle = false;
        } else if (((1LL << start) & board -> whiteRooks & (1LL << CASTLE_ROOKS[1])) != 0) {
            nextBoard -> whiteKingQueensideCastle = false;
        } else if (((1LL << start) & board -> blackRooks & (1LL << CASTLE_ROOKS[2])) != 0) {
            nextBoard -> blackKingKingsideCastle = false;
        } else if (((1LL << start) & board -> blackRooks & 1LL) != 0) {
            nextBoard -> blackKingQueensideCastle = false;
        }
    }
}

int64_t move(int64_t board, struct Move move, enum moveInfo type) {
    int start, end;
    if (move.endFile <= 7) {
        start = move.startRank * 8 + move.startFile;
        end = move.endRank * 8 + move.endFile;
        if (((board >> start) & 1) == 1) {
            board &= ~(1LL << start);
            board |= 1LL << end;
        } else {
            board &= ~(1LL << end);
        }
    } else if (move.endFile == PAWN_PROMOTION) {
        if (move.endRank >= WHITE_KNIGHT && move.endRank <= WHITE_QUEEN) {
            start = __builtin_ctzll(FILE_MASKS[move.startRank] & RANK_MASKS[1]);
            end = __builtin_ctzll(FILE_MASKS[move.startFile] & RANK_MASKS[0]);
        } else {
            start = __builtin_ctzll(FILE_MASKS[move.startRank] & RANK_MASKS[6]);
            end = __builtin_ctzll(FILE_MASKS[move.startFile] & RANK_MASKS[7]);
        }
        if (type == move.endRank) {
            board |= 1LL << end;
        } else {
            board &= ~(1LL << start);
            board &= ~(1LL << end);
        }
    } else if (move.endFile == EN_PASSANT) {
        if (move.endRank == WHITE) {
            start = __builtin_ctzll(FILE_MASKS[move.startRank] & RANK_MASKS[3]);
            end = __builtin_ctzll(FILE_MASKS[move.startFile] & RANK_MASKS[2]);
            board &= ~(FILE_MASKS[move.startFile] & RANK_MASKS[3]);
        } else {
            start = __builtin_ctzll(FILE_MASKS[move.startRank] & RANK_MASKS[4]);
            end = __builtin_ctzll(FILE_MASKS[move.startFile] & RANK_MASKS[5]);
            board &= ~(FILE_MASKS[move.startFile] & RANK_MASKS[4]);
        }
        if (((board >> start) & 1) == 1) {
            board &= ~(1LL << start);
            board |= (1LL << end);
        }

    }
    return board;
}

int64_t makeMoveEnPassant(int64_t board, struct Move move) {
    if (move.endFile <= 7) {
        int start = move.startRank * 8 + move.startFile;
        if ((abs(move.startRank - move.endRank) == 2) && (((board >> start) & 1) == 1)) { //pawn double push
            return FILE_MASKS[move.startFile - '0'];
        }
    }
    return 0;
}

int64_t makeMoveCastle(int64_t rookBoard, int64_t kingBoard, struct Move move, enum moveInfo type) {
    int start = move.startRank * 8 + move.startFile;
    bool isCastleMove = (move.startRank == 0 && move.startFile == 4 && move.endRank == 0) ||
        (move.startRank == 7 && move.startFile == 4 && move.endRank == 7) &&
        (move.endFile == 2 || move.endFile == 6);
    if (((kingBoard >> start) & 1) == 1 && isCastleMove) { //'regular' move
        if (type == WHITE_ROOK) {
            switch (move.endFile) {
            case 2:
                rookBoard &= ~(1LL << CASTLE_ROOKS[1]);
                rookBoard |= (1LL << (CASTLE_ROOKS[1] + 3));
                break;
            case 6:
                rookBoard &= ~(1LL << CASTLE_ROOKS[0]);
                rookBoard |= (1LL << (CASTLE_ROOKS[0] - 2));
                break;
            }
        } else {
            switch (move.endFile) {
            case 2:
                rookBoard &= ~(1LL << CASTLE_ROOKS[3]);
                rookBoard |= (1LL << (CASTLE_ROOKS[3] + 3));
                break;
            case 6:
                rookBoard &= ~(1LL << CASTLE_ROOKS[2]);
                rookBoard |= (1LL << (CASTLE_ROOKS[2] - 2));
                break;
            }
        }
    }
    return rookBoard;
}

void possibleMovesWhite(struct MoveList * moves, struct Board * board) {
    int64_t wp = board -> whitePawns;
    int64_t wn = board -> whiteKnights;
    int64_t wb = board -> whiteBishops;
    int64_t wr = board -> whiteRooks;
    int64_t wq = board -> whiteQueens;
    int64_t wk = board -> whiteKing;

    int64_t bp = board -> blackPawns;
    int64_t bn = board -> blackKnights;
    int64_t bb = board -> blackBishops;
    int64_t br = board -> blackRooks;
    int64_t bq = board -> blackQueens;
    int64_t bk = board -> blackKing;

    int64_t ep = board -> enPassant;

    WHITE_PIECES = (wp | wn | wb | wr | wq | wk);
    BLACK_PIECES = (bp | bn | bb | br | bq | bk);
    CAN_CAPTURE = ~(WHITE_PIECES | bk);

    OCCUPIED = WHITE_PIECES | BLACK_PIECES;
    EMPTY = ~OCCUPIED;

    possiblePawnMovesWhite(moves, wp, bp, ep);
    possibleKnightMoves(moves, wn);
    possibleBishopMoves(moves, wb);
    possibleRookMoves(moves, wr);
    possibleQueenMoves(moves, wq);
    possibleKingMovesWhite(moves, wp, wn, wb, wr, wq, wk, bp, bn, bb, br, bq, bk);
    possibleCastleWhite(moves, wp, wn, wb, wr, wq, wk, bp, bn, bb, br, bq, bk, board -> whiteKingKingsideCastle, board -> whiteKingQueensideCastle);
}

void possibleMovesBlack(struct MoveList * moves, struct Board * board) {
    int64_t wp = board -> whitePawns;
    int64_t wn = board -> whiteKnights;
    int64_t wb = board -> whiteBishops;
    int64_t wr = board -> whiteRooks;
    int64_t wq = board -> whiteQueens;
    int64_t wk = board -> whiteKing;

    int64_t bp = board -> blackPawns;
    int64_t bn = board -> blackKnights;
    int64_t bb = board -> blackBishops;
    int64_t br = board -> blackRooks;
    int64_t bq = board -> blackQueens;
    int64_t bk = board -> blackKing;

    int64_t ep = board -> enPassant;

    WHITE_PIECES = (wp | wn | wb | wr | wq | wk);
    BLACK_PIECES = (bp | bn | bb | br | bq | bk);

    CAN_CAPTURE = ~(BLACK_PIECES | wk);

    OCCUPIED = WHITE_PIECES | BLACK_PIECES;
    EMPTY = ~OCCUPIED;

    possiblePawnMovesBlack(moves, bp, wp, ep);
    possibleKnightMoves(moves, bn);
    possibleBishopMoves(moves, bb);
    possibleRookMoves(moves, br);
    possibleQueenMoves(moves, bq);
    possibleKingMovesBlack(moves, wp, wn, wb, wr, wq, wk, bp, bn, bb, br, bq, bk);
    possibleCastleBlack(moves, wp, wn, wb, wr, wq, wk, bp, bn, bb, br, bq, bk, board -> blackKingKingsideCastle, board -> blackKingQueensideCastle);
}

void possiblePawnMovesWhite(struct MoveList * moves, int64_t wp, int64_t bp, int64_t ep) {
    int64_t PAWN_MOVES = (wp >> 7) & CAN_CAPTURE & OCCUPIED & ~RANK_8 & ~FILE_A;
    int64_t possibility = PAWN_MOVES & -PAWN_MOVES;
    while (possibility != 0) {
        int i = __builtin_ctzll(possibility);
        if (((PAWN_MOVES >> i) & 1) == 1) {
            addMove(moves, i / 8 + 1, i % 8 - 1, i / 8, i % 8);
        }
        PAWN_MOVES &= ~possibility;
        possibility = PAWN_MOVES & -PAWN_MOVES;
    }
    PAWN_MOVES = (wp >> 9) & CAN_CAPTURE & OCCUPIED & ~RANK_8 & ~FILE_H;
    possibility = PAWN_MOVES & -PAWN_MOVES;
    while (possibility != 0) {
        int i = __builtin_ctzll(possibility);
        if (((PAWN_MOVES >> i) & 1) == 1) {
            addMove(moves, i / 8 + 1, i % 8 + 1, i / 8, i % 8);
        }
        PAWN_MOVES &= ~possibility;
        possibility = PAWN_MOVES & -PAWN_MOVES;
    }
    PAWN_MOVES = (wp >> 8) & EMPTY & ~RANK_8;
    possibility = PAWN_MOVES & -PAWN_MOVES;
    while (possibility != 0) {
        int i = __builtin_ctzll(possibility);
        if (((PAWN_MOVES >> i) & 1) == 1) {
            addMove(moves, i / 8 + 1, i % 8, i / 8, i % 8);
        }
        PAWN_MOVES &= ~possibility;
        possibility = PAWN_MOVES & -PAWN_MOVES;
    }
    PAWN_MOVES = (wp >> 16) & EMPTY & (EMPTY >> 8) & RANK_4;
    possibility = PAWN_MOVES & -PAWN_MOVES;
    while (possibility != 0) {
        int i = __builtin_ctzll(possibility);
        if (((PAWN_MOVES >> i) & 1) == 1) {
            addMove(moves, i / 8 + 2, i % 8, i / 8, i % 8);
        }
        PAWN_MOVES &= ~possibility;
        possibility = PAWN_MOVES & -PAWN_MOVES;
    }

    PAWN_MOVES = (wp >> 7) & CAN_CAPTURE & OCCUPIED & RANK_8 & ~FILE_A;
    possibility = PAWN_MOVES & -PAWN_MOVES;
    while (possibility != 0) {
        int i = __builtin_ctzll(possibility);
        int rank = i % 8 - 1, rank2 = i % 8;
        addMove(moves, rank, rank2, WHITE_QUEEN, PAWN_PROMOTION);
        addMove(moves, rank, rank2, WHITE_ROOK, PAWN_PROMOTION);
        addMove(moves, rank, rank2, WHITE_BISHOP, PAWN_PROMOTION);
        addMove(moves, rank, rank2, WHITE_KNIGHT, PAWN_PROMOTION);
        PAWN_MOVES &= ~possibility;
        possibility = PAWN_MOVES & -PAWN_MOVES;
    }
    PAWN_MOVES = (wp >> 9) & CAN_CAPTURE & OCCUPIED & RANK_8 & ~FILE_H;
    possibility = PAWN_MOVES & -PAWN_MOVES;
    while (possibility != 0) {
        int i = __builtin_ctzll(possibility);
        int rank = i % 8 + 1, rank2 = i % 8;
        addMove(moves, rank, rank2, WHITE_QUEEN, PAWN_PROMOTION);
        addMove(moves, rank, rank2, WHITE_ROOK, PAWN_PROMOTION);
        addMove(moves, rank, rank2, WHITE_BISHOP, PAWN_PROMOTION);
        addMove(moves, rank, rank2, WHITE_KNIGHT, PAWN_PROMOTION);
        PAWN_MOVES &= ~possibility;
        possibility = PAWN_MOVES & -PAWN_MOVES;
    }
    PAWN_MOVES = (wp >> 8) & EMPTY & RANK_8;
    possibility = PAWN_MOVES & -PAWN_MOVES;
    while (possibility != 0) {
        int i = __builtin_ctzll(possibility);
        int rank = i % 8, rank2 = i % 8;
        addMove(moves, rank, rank2, WHITE_QUEEN, PAWN_PROMOTION);
        addMove(moves, rank, rank2, WHITE_ROOK, PAWN_PROMOTION);
        addMove(moves, rank, rank2, WHITE_BISHOP, PAWN_PROMOTION);
        addMove(moves, rank, rank2, WHITE_KNIGHT, PAWN_PROMOTION);
        PAWN_MOVES &= ~possibility;
        possibility = PAWN_MOVES & -PAWN_MOVES;
    }

    possibility = (wp << 1) & bp & RANK_5 & ~FILE_A & ep; //shows piece to remove, not the destination
    if (possibility != 0) {
        int index = __builtin_ctzll(possibility);
        addMove(moves, index % 8 - 1, index % 8, WHITE, EN_PASSANT);
    }
    //en passant left
    possibility = (wp >> 1) & bp & RANK_5 & ~FILE_H & ep; //shows piece to remove, not the destination
    if (possibility != 0) {
        int index = __builtin_ctzll(possibility);
        addMove(moves, index % 8 + 1, index % 8, WHITE, EN_PASSANT);
    }
}

void possiblePawnMovesBlack(struct MoveList * moves, int64_t wp, int64_t bp, int64_t ep) {
    int64_t PAWN_MOVES = (wp << 7) & CAN_CAPTURE & OCCUPIED & ~RANK_1 & ~FILE_H;
    int64_t possibility = PAWN_MOVES & -PAWN_MOVES;
    while (possibility != 0) {
        int i = __builtin_ctzll(possibility);
        if (((PAWN_MOVES >> i) & 1) == 1) {
            addMove(moves, i / 8 - 1, i % 8 + 1, i / 8, i % 8);
        }
        PAWN_MOVES &= ~possibility;
        possibility = PAWN_MOVES & -PAWN_MOVES;
    }
    PAWN_MOVES = (wp << 9) & CAN_CAPTURE & OCCUPIED & ~RANK_1 & ~FILE_A;
    possibility = PAWN_MOVES & -PAWN_MOVES;
    while (possibility != 0) {
        int i = __builtin_ctzll(possibility);
        if (((PAWN_MOVES >> i) & 1) == 1) {
            addMove(moves, i / 8 - 1, i % 8 - 1, i / 8, i % 8);
        }
        PAWN_MOVES &= ~possibility;
        possibility = PAWN_MOVES & -PAWN_MOVES;
    }
    PAWN_MOVES = (wp << 8) & EMPTY & ~RANK_1;
    possibility = PAWN_MOVES & -PAWN_MOVES;
    while (possibility != 0) {
        int i = __builtin_ctzll(possibility);
        if (((PAWN_MOVES >> i) & 1) == 1) {
            addMove(moves, i / 8 - 1, i % 8, i / 8, i % 8);
        }
        PAWN_MOVES &= ~possibility;
        possibility = PAWN_MOVES & -PAWN_MOVES;
    }
    PAWN_MOVES = (wp << 16) & EMPTY & (EMPTY << 8) & RANK_5;
    possibility = PAWN_MOVES & -PAWN_MOVES;
    while (possibility != 0) {
        int i = __builtin_ctzll(possibility);
        if (((PAWN_MOVES >> i) & 1) == 1) {
            addMove(moves, i / 8 - 2, i % 8, i / 8, i % 8);
        }
        PAWN_MOVES &= ~possibility;
        possibility = PAWN_MOVES & -PAWN_MOVES;
    }

    PAWN_MOVES = (wp << 7) & CAN_CAPTURE & OCCUPIED & RANK_1 & ~FILE_H;
    possibility = PAWN_MOVES & -PAWN_MOVES;
    while (possibility != 0) {
        int i = __builtin_ctzll(possibility);
        int rank = i % 8 + 1, rank2 = i % 8;
        addMove(moves, rank, rank2, BLACK_QUEEN, PAWN_PROMOTION);
        addMove(moves, rank, rank2, BLACK_ROOK, PAWN_PROMOTION);
        addMove(moves, rank, rank2, BLACK_BISHOP, PAWN_PROMOTION);
        addMove(moves, rank, rank2, BLACK_KNIGHT, PAWN_PROMOTION);
        PAWN_MOVES &= ~possibility;
        possibility = PAWN_MOVES & -PAWN_MOVES;
    }
    PAWN_MOVES = (wp << 9) & CAN_CAPTURE & OCCUPIED & RANK_1 & ~FILE_A;
    possibility = PAWN_MOVES & -PAWN_MOVES;
    while (possibility != 0) {
        int i = __builtin_ctzll(possibility);
        int rank = i % 8 - 1, rank2 = i % 8;
        addMove(moves, rank, rank2, BLACK_QUEEN, PAWN_PROMOTION);
        addMove(moves, rank, rank2, BLACK_ROOK, PAWN_PROMOTION);
        addMove(moves, rank, rank2, BLACK_BISHOP, PAWN_PROMOTION);
        addMove(moves, rank, rank2, BLACK_KNIGHT, PAWN_PROMOTION);
        PAWN_MOVES &= ~possibility;
        possibility = PAWN_MOVES & -PAWN_MOVES;
    }
    PAWN_MOVES = (wp << 8) & EMPTY & RANK_1;
    possibility = PAWN_MOVES & -PAWN_MOVES;
    while (possibility != 0) {
        int i = __builtin_ctzll(possibility);
        int rank = i % 8, rank2 = i % 8;
        addMove(moves, rank, rank2, BLACK_QUEEN, PAWN_PROMOTION);
        addMove(moves, rank, rank2, BLACK_ROOK, PAWN_PROMOTION);
        addMove(moves, rank, rank2, BLACK_BISHOP, PAWN_PROMOTION);
        addMove(moves, rank, rank2, BLACK_KNIGHT, PAWN_PROMOTION);
        PAWN_MOVES &= ~possibility;
        possibility = PAWN_MOVES & -PAWN_MOVES;
    }

    possibility = (wp >> 1) & bp & RANK_4 & ~FILE_H & ep; //shows piece to remove, not the destination
    if (possibility != 0) {
        int index = __builtin_ctzll(possibility);
        addMove(moves, index % 8 + 1, index % 8, BLACK, EN_PASSANT);
    }
    //en passant left
    possibility = (wp << 1) & bp & RANK_4 & ~FILE_A & ep; //shows piece to remove, not the destination
    if (possibility != 0) {
        int index = __builtin_ctzll(possibility);
        addMove(moves, index % 8 - 1, index % 8, BLACK, EN_PASSANT);
    }
}

void possibleKnightMoves(struct MoveList * moves, int64_t wn) {
    int64_t i = wn & -wn;
    int64_t possibility;
    while (i != 0) {
        int location = __builtin_ctzll(i);
        if (location > 18) {
            possibility = KNIGHT_SPAN << (location - 18);
        } else {
            possibility = KNIGHT_SPAN >> (18 - location);
        }
        if (location % 8 < 4) {
            possibility &= ~FILE_GH & CAN_CAPTURE;
        } else {
            possibility &= ~FILE_AB & CAN_CAPTURE;
        }
        int64_t j = possibility & -possibility;
        while (j != 0) {
            int index = __builtin_ctzll(j);
            addMove(moves, location / 8, location % 8, index / 8, index % 8);
            possibility &= ~j;
            j = possibility & -possibility;
        }
        wn &= ~i;
        i = wn & -wn;
    }
}

void possibleBishopMoves(struct MoveList * moves, int64_t wb) {
    int64_t i = wb & -wb;
    int64_t possibility;
    while (i != 0) {
        int location = __builtin_ctzll(i);
        possibility = diagonalMoves(location) & CAN_CAPTURE;
        int64_t j = possibility & -possibility;
        while (j != 0) {
            int location2 = __builtin_ctzll(j);
            addMove(moves, location / 8, location % 8, location2 / 8, location2 % 8);
            possibility &= ~j;
            j = possibility & -possibility;
        }
        wb &= ~i;
        i = wb & -wb;
    }
}

void possibleRookMoves(struct MoveList * moves, int64_t wr) {
    int64_t i = wr & -wr;
    int64_t possibility;
    while (i != 0) {
        int location = __builtin_ctzll(i);
        possibility = straightMoves(location) & CAN_CAPTURE;
        int64_t j = possibility & -possibility;
        while (j != 0) {
            int location2 = __builtin_ctzll(j);
            addMove(moves, location / 8, location % 8, location2 / 8, location2 % 8);
            possibility &= ~j;
            j = possibility & -possibility;
        }
        wr &= ~i;
        i = wr & -wr;
    }
}

void possibleQueenMoves(struct MoveList * moves, int64_t wq) {
    int64_t i = wq & -wq;
    int64_t possibility;
    while (i != 0) {
        int location = __builtin_ctzll(i);
        possibility = (straightMoves(location) | diagonalMoves(location)) & CAN_CAPTURE;
        int64_t j = possibility & -possibility;
        while (j != 0) {
            int location2 = __builtin_ctzll(j);
            addMove(moves, location / 8, location % 8, location2 / 8, location2 % 8);
            possibility &= ~j;
            j = possibility & -possibility;
        }
        wq &= ~i;
        i = wq & -wq;
    }
}

void possibleKingMovesWhite(struct MoveList * moves, int64_t wp, int64_t wn, int64_t wb, int64_t wr, int64_t wq, int64_t wk, int64_t bp, int64_t bn, int64_t bb, int64_t br, int64_t bq, int64_t bk) {
    int64_t possibility;
    int iLocation = __builtin_ctzll(wk);
    if (iLocation > 9) {
        possibility = KING_SPAN << (iLocation - 9);
    } else {
        possibility = KING_SPAN >> (9 - iLocation);
    }
    if (iLocation % 8 < 4) {
        possibility &= ~FILE_GH & CAN_CAPTURE;
    } else {
        possibility &= ~FILE_AB & CAN_CAPTURE;
    }
    int64_t SAFE = ~blackAttacks(wp, wn, wb, wr, wq, wk, bp, bn, bb, br, bq, bk);
    possibility = possibility & SAFE;
    int64_t j = possibility & ~(possibility - 1);
    while (j != 0) {
        int index = __builtin_ctzll(j);
        addMove(moves, iLocation / 8, iLocation % 8, index / 8, index % 8);
        possibility &= ~j;
        j = possibility & ~(possibility - 1);
    }
}

void possibleKingMovesBlack(struct MoveList * moves, int64_t wp, int64_t wn, int64_t wb, int64_t wr, int64_t wq, int64_t wk, int64_t bp, int64_t bn, int64_t bb, int64_t br, int64_t bq, int64_t bk) {
    int64_t possibility;
    int iLocation = __builtin_ctzll(bk);
    if (iLocation > 9) {
        possibility = KING_SPAN << (iLocation - 9);
    } else {
        possibility = KING_SPAN >> (9 - iLocation);
    }
    if (iLocation % 8 < 4) {
        possibility &= ~FILE_GH & CAN_CAPTURE;
    } else {
        possibility &= ~FILE_AB & CAN_CAPTURE;
    }
    int64_t SAFE = ~whiteAttacks(wp, wn, wb, wr, wq, wk, bp, bn, bb, br, bq, bk);
    possibility = possibility & SAFE;
    int64_t j = possibility & ~(possibility - 1);
    while (j != 0) {
        int index = __builtin_ctzll(j);
        addMove(moves, iLocation / 8, iLocation % 8, index / 8, index % 8);
        possibility &= ~j;
        j = possibility & ~(possibility - 1);
    }
}

int64_t whiteAttacks(int64_t wp, int64_t wn, int64_t wb, int64_t wr, int64_t wq, int64_t wk, int64_t bp, int64_t bn, int64_t bb, int64_t br, int64_t bq, int64_t bk) {
    WHITE_PIECES = (wp | wn | wb | wr | wq | wk);
    BLACK_PIECES = (bp | bn | bb | br | bq | bk);
    OCCUPIED = WHITE_PIECES | BLACK_PIECES;
    int64_t unsafe;
    //pawn
    unsafe = ((wp >> 7) & ~FILE_A); //pawn capture right
    unsafe |= ((wp >> 9) & ~FILE_H); //pawn capture left
    int64_t possibility;
    //knight
    int64_t i = wn & -wn;
    while (i != 0) {
        int location = __builtin_ctzll(i);
        if (location > 18) {
            possibility = KNIGHT_SPAN << (location - 18);
        } else {
            possibility = KNIGHT_SPAN >> (18 - location);
        }
        if (location % 8 < 4) {
            possibility &= ~FILE_GH;
        } else {
            possibility &= ~FILE_AB;
        }
        unsafe |= possibility;
        wn &= ~i;
        i = wn & -wn;
    }
    //bishop/queen
    int64_t qb = wq | wb;
    i = qb & -qb;
    while (i != 0) {
        int location = __builtin_ctzll(i);
        possibility = diagonalMoves(location);
        unsafe |= possibility;
        qb &= ~i;
        i = qb & -qb;
    }
    //rook/queen
    int64_t qr = wq | wr;
    i = qr & -qr;
    while (i != 0) {
        int location = __builtin_ctzll(i);
        possibility = straightMoves(location);
        unsafe |= possibility;
        qr &= ~i;
        i = qr & -qr;
    }
    //king
    int location = __builtin_ctzll(wk);
    if (location > 9) {
        possibility = KING_SPAN << (location - 9);
    } else {
        possibility = KING_SPAN >> (9 - location);
    }
    if (location % 8 < 4) {
        possibility &= ~FILE_GH;
    } else {
        possibility &= ~FILE_AB;
    }
    unsafe |= possibility;
    return unsafe;
}

int64_t blackAttacks(int64_t wp, int64_t wn, int64_t wb, int64_t wr, int64_t wq, int64_t wk, int64_t bp, int64_t bn, int64_t bb, int64_t br, int64_t bq, int64_t bk) {
    WHITE_PIECES = (wp | wn | wb | wr | wq | wk);
    BLACK_PIECES = (bp | bn | bb | br | bq | bk);
    OCCUPIED = WHITE_PIECES | BLACK_PIECES;
    int64_t unsafe;
    //pawn
    unsafe = ((bp << 7) & ~FILE_H); //pawn capture right
    unsafe |= ((bp << 9) & ~FILE_A); //pawn capture left
    int64_t possibility;
    //knight
    int64_t i = bn & -bn;
    while (i != 0) {
        int location = __builtin_ctzll(i);
        if (location > 18) {
            possibility = KNIGHT_SPAN << (location - 18);
        } else {
            possibility = KNIGHT_SPAN >> (18 - location);
        }
        if (location % 8 < 4) {
            possibility &= ~FILE_GH;
        } else {
            possibility &= ~FILE_AB;
        }
        unsafe |= possibility;
        bn &= ~i;
        i = bn & -bn;
    }
    //bishop/queen
    int64_t qb = bq | bb;
    i = qb & -qb;
    while (i != 0) {
        int location = __builtin_ctzll(i);
        possibility = diagonalMoves(location);
        unsafe |= possibility;
        qb &= ~i;
        i = qb & -qb;
    }
    //rook/queen
    int64_t qr = bq | br;
    i = qr & -qr;
    while (i != 0) {
        int location = __builtin_ctzll(i);
        possibility = straightMoves(location);
        unsafe |= possibility;
        qr &= ~i;
        i = qr & -qr;
    }
    //king
    int location = __builtin_ctzll(bk);
    if (location > 9) {
        possibility = KING_SPAN << (location - 9);
    } else {
        possibility = KING_SPAN >> (9 - location);
    }
    if (location % 8 < 4) {
        possibility &= ~FILE_GH;
    } else {
        possibility &= ~FILE_AB;
    }
    unsafe |= possibility;
    return unsafe;
}

void possibleCastleWhite(struct MoveList * moves, int64_t wp, int64_t wn, int64_t wb, int64_t wr, int64_t wq, int64_t wk, int64_t bp, int64_t bn, int64_t bb, int64_t br, int64_t bq, int64_t bk, bool wck, bool wcq) {
    if (wck && ((1LL << CASTLE_ROOKS[0]) & wr) != 0 && (OCCUPIED & ((1LL << 61) | (1LL << 62))) == 0) {
        int64_t UNSAFE = blackAttacks(wp, wn, wb, wr, wq, wk, bp, bn, bb, br, bq, bk);
        if (((wk | (1LL << 61) | (1LL << 62)) & UNSAFE) == 0) {
            addMove(moves, 7, 4, 7, 6);
        }
    }
    if (wcq && ((1LL << CASTLE_ROOKS[1]) & wr) != 0 && (OCCUPIED & ((1LL << 57) | (1LL << 58) | (1LL << 59))) == 0) {
        int64_t UNSAFE = blackAttacks(wp, wn, wb, wr, wq, wk, bp, bn, bb, br, bq, bk);
        if (((wk | (1LL << 58) | (1LL << 59)) & UNSAFE) == 0) {
            addMove(moves, 7, 4, 7, 2);
        }
    }
}

void possibleCastleBlack(struct MoveList * moves, int64_t wp, int64_t wn, int64_t wb, int64_t wr, int64_t wq, int64_t wk, int64_t bp, int64_t bn, int64_t bb, int64_t br, int64_t bq, int64_t bk, bool wck, bool wcq) {
    if (wck && ((1LL << CASTLE_ROOKS[2]) & br) != 0 && (OCCUPIED & ((1LL << 5) | (1LL << 6))) == 0) {
        int64_t UNSAFE = whiteAttacks(wp, wn, wb, wr, wq, wk, bp, bn, bb, br, bq, bk);
        if (((bk | (1LL << 5) | (1LL << 6)) & UNSAFE) == 0) {
            addMove(moves, 0, 4, 0, 6);
        }
    }
    if (wcq && ((1LL << CASTLE_ROOKS[3]) & br) != 0 && (OCCUPIED & ((1LL << 1) | (1LL << 2) | (1LL << 3))) == 0) {
        int64_t UNSAFE = whiteAttacks(wp, wn, wb, wr, wq, wk, bp, bn, bb, br, bq, bk);
        if (((bk | (1LL << 2) | (1LL << 3)) & UNSAFE) == 0) {
            addMove(moves, 0, 4, 0, 6);
        }
    }
}

int64_t straightMoves(int location) {
    int64_t locationMask = 1LL << location;
    int64_t movesHorizontal = (OCCUPIED - 2 * locationMask) ^ reverse(reverse(OCCUPIED) - 2 * reverse(locationMask));
    int64_t movesVertical = ((OCCUPIED & FILE_MASKS[location % 8]) - (2 * locationMask)) ^ reverse(reverse(OCCUPIED & FILE_MASKS[location % 8]) - (2 * reverse(locationMask)));
    return (movesHorizontal & RANK_MASKS[location / 8]) | (movesVertical & FILE_MASKS[location % 8]);
}

int64_t diagonalMoves(int location) {
    int64_t locationMask = 1LL << location;
    int64_t movesDiagonal = ((OCCUPIED & DIAGONAL_MASKS[(location / 8) + (location % 8)]) - (2 * locationMask)) ^ reverse(reverse(OCCUPIED & DIAGONAL_MASKS[(location / 8) + (location % 8)]) - (2 * reverse(locationMask)));
    int64_t movesAntiDiagonal = ((OCCUPIED & ANTI_DIAGONAL_MASKS[(location / 8) + 7 - (location % 8)]) - (2 * locationMask)) ^ reverse(reverse(OCCUPIED & ANTI_DIAGONAL_MASKS[(location / 8) + 7 - (location % 8)]) - (2 * reverse(locationMask)));
    return (movesDiagonal & DIAGONAL_MASKS[(location / 8) + (location % 8)]) | (movesAntiDiagonal & ANTI_DIAGONAL_MASKS[(location / 8) + 7 - (location % 8)]);
}

bool checkCheckmate(struct Board * board, bool WhiteToMove) {
    int64_t wp = board -> whitePawns;
    int64_t wn = board -> whiteKnights;
    int64_t wb = board -> whiteBishops;
    int64_t wr = board -> whiteRooks;
    int64_t wq = board -> whiteQueens;
    int64_t wk = board -> whiteKing;

    int64_t bp = board -> blackPawns;
    int64_t bn = board -> blackKnights;
    int64_t bb = board -> blackBishops;
    int64_t br = board -> blackRooks;
    int64_t bq = board -> blackQueens;
    int64_t bk = board -> blackKing;

    int64_t ep = board -> enPassant;

    WHITE_PIECES = (wp | wn | wb | wr | wq | wk);
    BLACK_PIECES = (bp | bn | bb | br | bq | bk);
    
    int64_t blackAttacksL = blackAttacks(wp, wn, wb, wr, wq, 0LL, bp, bn, bb, br, bq, bk);
    int64_t whiteAttacksL = whiteAttacks(wp, wn, wb, wr, wq, wk, bp, bn, bb, br, bq, 0LL);
    
    if (WhiteToMove && (wk & blackAttacksL)) {
        int64_t allowed = ~(blackAttacksL|WHITE_PIECES);
        int iLocation = __builtin_ctzll(wk);
        CAN_CAPTURE = ~(WHITE_PIECES | bk);
        int64_t possibility;
        if (iLocation > 9) {
            possibility = KING_SPAN << (iLocation - 9);
        } else {
            possibility = KING_SPAN >> (9 - iLocation);
        }
        if (iLocation % 8 < 4) {
            possibility &= ~FILE_GH & CAN_CAPTURE;
        } else {
            possibility &= ~FILE_AB & CAN_CAPTURE;
        }
        if(possibility&allowed) {
            return false;
        }
    }
    else if (!WhiteToMove && (bk & whiteAttacksL)) {
        int64_t allowed = ~(blackAttacksL|BLACK_PIECES);
        int iLocation = __builtin_ctzll(bk);
        CAN_CAPTURE = ~(BLACK_PIECES | wk);
        int64_t possibility;
        if (iLocation > 9) {
            possibility = KING_SPAN << (iLocation - 9);
        } else {
            possibility = KING_SPAN >> (9 - iLocation);
        }
        if (iLocation % 8 < 4) {
            possibility &= ~FILE_GH & CAN_CAPTURE;
        } else {
            possibility &= ~FILE_AB & CAN_CAPTURE;
        }
        if(possibility&allowed) {
            return false;
        }
    }
    else {
        return false;
    }
    
    //printf("maybe a mate\n");
    //only thing left to do is trying moves
    
    struct MoveList moves;
    struct Board nextBoard;
    
    moves.size = 0;
    if (WhiteToMove) {
        possibleMovesWhite( & moves, board);
    } else {
        possibleMovesBlack( & moves, board);
    }
    
    for (int i = 0; i < moves.size; i++) {
        applyMove( board, &nextBoard, moves.moves[i] );

        if (WhiteToMove && !(nextBoard.whiteKing & blackAttacks(nextBoard.whitePawns, nextBoard.whiteKnights, nextBoard.whiteBishops, nextBoard.whiteRooks, nextBoard.whiteQueens, nextBoard.whiteKing, nextBoard.blackPawns, nextBoard.blackKnights, nextBoard.blackBishops, nextBoard.blackRooks, nextBoard.blackQueens, nextBoard.blackKing))) {
            return false;
        }

        if (!WhiteToMove && !(nextBoard.blackKing & whiteAttacks(nextBoard.whitePawns, nextBoard.whiteKnights, nextBoard.whiteBishops, nextBoard.whiteRooks, nextBoard.whiteQueens, nextBoard.whiteKing, nextBoard.blackPawns, nextBoard.blackKnights, nextBoard.blackBishops, nextBoard.blackRooks, nextBoard.blackQueens, nextBoard.blackKing))) {
            return false;
        }
    }
    
    return true;
}

void addMove(struct MoveList * moves, enum moveInfo startRank, enum moveInfo startFile, enum moveInfo endRank, enum moveInfo endFile) {
    (moves -> moves[moves -> size]).startRank = startRank;
    (moves -> moves[moves -> size]).startFile = startFile;
    (moves -> moves[moves -> size]).endRank = endRank;
    (moves -> moves[moves -> size]).endFile = endFile;
    moves -> size++;
}

int64_t reverse(int64_t bits) {
    //funny bits moment
    // swap odd and even bits
    bits = ((bits >> 1) & 0x5555555555555555LL) | ((bits & 0x5555555555555555LL) << 1);
    // swap consecutive pairs
    bits = ((bits >> 2) & 0x3333333333333333LL) | ((bits & 0x3333333333333333LL) << 2);
    // swap nibbles ...
    bits = ((bits >> 4) & 0x0F0F0F0F0F0F0F0FLL) | ((bits & 0x0F0F0F0F0F0F0F0FLL) << 4);
    // swap bytes
    bits = ((bits >> 8) & 0x00FF00FF00FF00FFLL) | ((bits & 0x00FF00FF00FF00FFLL) << 8);
    // swap 2-byte long pairs
    bits = ((bits >> 16) & 0x0000FFFF0000FFFFLL) | ((bits & 0x0000FFFF0000FFFFLL) << 16);
    // swap 4-byte long pairs
    bits = (bits >> 32) | (bits << 32);
    return bits;
}
