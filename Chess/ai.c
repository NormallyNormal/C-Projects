#include "moves.h"

#include "ai.h"

#include <stdlib.h>

#include <stdio.h>

#include <stdbool.h>

#include <math.h>

struct MoveSortable {
    struct Move move;
    double value;
};

#define evalTableSize 44753627
#define ZOBRIST_DEBUG

uint64_t * evalTableKey;
double * evalTable;
#ifdef ZOBRIST_DEBUG
struct Board * evalTableBoards;
#endif

static int64_t zArray[2][6][64];
static int64_t zEnPassant[8];
static int64_t zCastle[4];
static int64_t zBlackMove;

static int64_t count;

static int hits;

static int firstDepth;

void init(void) {
    evalTableKey = malloc(evalTableSize * sizeof(int64_t));
    evalTable = malloc(evalTableSize * sizeof(double));
    #ifdef ZOBRIST_DEBUG
    evalTableBoards = malloc(evalTableSize * sizeof(struct Board));
    #endif
    genZobrist();
}

bool boardCompare(struct Board * board, struct Board * board2) {
    if(board->whitePawns != board2->whitePawns) {
        printf("WP ");
        return false;
    }
    if(board->whiteKnights != board2->whiteKnights) {
        printf("WN ");
        return false;
    }
    if(board->whiteBishops != board2->whiteBishops) {
        printf("WB ");
        return false;
    }
    if(board->whiteRooks != board2->whiteRooks) {
        printf("WR ");
        return false;
    }
    if(board->whiteQueens != board2->whiteQueens) {
        printf("WQ ");
        return false;
    }
    if(board->whiteKing != board2->whiteKing) {
        printf("WK ");
        return false;
    }
    if(board->blackPawns != board2->blackPawns) {
        printf("BP ");
        return false;
    }
    if(board->blackKnights != board2->blackKnights) {
        printf("BN ");
        return false;
    }
    if(board->blackBishops != board2->blackBishops) {
        printf("BB ");
        return false;
    }
    if(board->blackRooks != board2->blackRooks) {
        printf("BR ");
        return false;
    }
    if(board->blackQueens != board2->blackQueens) {
        printf("BQ ");
        return false;
    }
    if(board->blackKing != board2->blackKing) {
        printf("BK ");
        return false;
    }
    if(board->enPassant != board2->enPassant) {
        printf("EP ");
        return false;
    }
    if(board-> whiteKingKingsideCastle != board2->whiteKingKingsideCastle) {
        printf("C ");
        return false;
    }
    if(board-> whiteKingQueensideCastle != board2->whiteKingQueensideCastle) {
    printf("C ");
        return false;
    }
    if(board-> blackKingKingsideCastle != board2->blackKingKingsideCastle) {
    printf("C ");
        return false;
    }
    if(board-> blackKingQueensideCastle != board2->blackKingQueensideCastle) {
    printf("C ");
        return false;
    }
    return true;
}

struct Move * findBest(struct Board * board, bool WhiteToMove, int depth) {
    hits = 0;
    firstDepth = depth;
    count = 0;
    struct MoveList moves;
    struct Board nextBoard;
    struct Move * bestMove = malloc(sizeof(struct Move));
    double bestMoveScore = -99999999999.0;
    moves.size = 0;
    if (WhiteToMove) {
        possibleMovesWhite( & moves, board);
    } else {
        possibleMovesBlack( & moves, board);
    }
    bubbleAdvanced(board, & moves, WhiteToMove);
    int validMoves = 0;
    for (int i = 0; i < moves.size; i++) {
        count++;
        applyMove( board, &nextBoard, moves.moves[i] );

        if (WhiteToMove && (nextBoard.whiteKing & blackAttacks(nextBoard.whitePawns, nextBoard.whiteKnights, nextBoard.whiteBishops, nextBoard.whiteRooks, nextBoard.whiteQueens, nextBoard.whiteKing, nextBoard.blackPawns, nextBoard.blackKnights, nextBoard.blackBishops, nextBoard.blackRooks, nextBoard.blackQueens, nextBoard.blackKing))) {
            continue;
        }

        if (!WhiteToMove && (nextBoard.blackKing & whiteAttacks(nextBoard.whitePawns, nextBoard.whiteKnights, nextBoard.whiteBishops, nextBoard.whiteRooks, nextBoard.whiteQueens, nextBoard.whiteKing, nextBoard.blackPawns, nextBoard.blackKnights, nextBoard.blackBishops, nextBoard.blackRooks, nextBoard.blackQueens, nextBoard.blackKing))) {
            continue;
        }

        validMoves++;
        
        uint64_t key = zobrist(& nextBoard, WhiteToMove);
        
        double eval;

        if(evalTableKey[key % evalTableSize] == key) {
            eval = evalTable[key % evalTableSize];
            if(eval > 1000.0) {
                eval = -99999.0 - depth;
            }
            #ifdef ZOBRIST_DEBUG
            if(!boardCompare(&evalTableBoards[key % evalTableSize], & nextBoard)) {
                printf("WTF! %ld %ld\n", evalTableKey[key % evalTableSize], key);
                printf("Recalc: %ld %ld\n", zobrist(& evalTableBoards[key % evalTableSize], WhiteToMove), key);
            }
            #endif
        }
        else {
            #ifdef ZOBRIST_DEBUG
            evalTableBoards[key % evalTableSize] = nextBoard;
            #endif
            eval = -negaMax( & nextBoard, !WhiteToMove, depth - 1, -9999999.0, 9999999.0);
            evalTable[key % evalTableSize] = eval;
            evalTableKey[key % evalTableSize] = key;
        }
        
        printf("%d%% - %d %d %d %d - %ld - %lf\n", (int)((float) i / moves.size * 100), moves.moves[i].startRank, moves.moves[i].startFile, moves.moves[i].endRank, moves.moves[i].endFile, count, eval);
        //printf(".");
        fflush(stdout);

        if (bestMoveScore < eval) {
            bestMoveScore = eval;
            * bestMove = moves.moves[i];
        }
    }

    for(int64_t i = 0; i < evalTableSize; i++) {
        evalTableKey[i] = 0;
        evalTableBoards[i].whitePawns = 0;
    }
    //printf("%d\n", hits);
    return bestMove;
}

double negaMax(struct Board * board, bool WhiteToMove, int depth, double alpha, double beta) {

    double eval;
    
    if(checkCheckmate(board, WhiteToMove)) {
        return -99999.0 - depth;
    } 
    
    //Maybe should do check check???
    if(depth >= 3) {
        eval = -negaMax( board, !WhiteToMove, depth - 1 - 2, -beta, -beta + 1);
        if(eval >= beta) {
           return beta;
        }
    }
    
    if (depth <= 0) {
        //return evaluate(board, WhiteToMove);
        return negaMaxCaptures(board, WhiteToMove, alpha, beta);
    }

    struct MoveList moves;
    struct Board nextBoard;
    
    //double eval = eval(board);
    moves.size = 0;
    if (WhiteToMove) {
        possibleMovesWhite( & moves, board);
    } else {
        possibleMovesBlack( & moves, board);
    }
    
    //change this if you want the ai to play as white future me
    if(!WhiteToMove) {
        prune(board, &moves, WhiteToMove);
    }
    
    if(depth > firstDepth - 2) {
        bubbleAdvanced(board, & moves, WhiteToMove);
    }
    else {
        bubble(board, & moves, WhiteToMove);
    }
    
    int validMoves = 0;
    for (int i = 0; i < moves.size; i++) {
        count++;
        applyMove( board, &nextBoard, moves.moves[i] );
        
        if (WhiteToMove && (nextBoard.whiteKing & blackAttacks(nextBoard.whitePawns, nextBoard.whiteKnights, nextBoard.whiteBishops, nextBoard.whiteRooks, nextBoard.whiteQueens, nextBoard.whiteKing, nextBoard.blackPawns, nextBoard.blackKnights, nextBoard.blackBishops, nextBoard.blackRooks, nextBoard.blackQueens, nextBoard.blackKing))) {
            continue;
        }
        else if (!WhiteToMove && (nextBoard.blackKing & whiteAttacks(nextBoard.whitePawns, nextBoard.whiteKnights, nextBoard.whiteBishops, nextBoard.whiteRooks, nextBoard.whiteQueens, nextBoard.whiteKing, nextBoard.blackPawns, nextBoard.blackKnights, nextBoard.blackBishops, nextBoard.blackRooks, nextBoard.blackQueens, nextBoard.blackKing))) {
            continue;
        }
        
        validMoves++;
        
        uint64_t key = zobrist(& nextBoard, WhiteToMove);
        if(evalTableKey[key % evalTableSize] == key) {
            hits++;
            if(eval > 1000.0) {
                eval = -99999.0 - depth;
            }
            eval = evalTable[key % evalTableSize];
            #ifdef ZOBRIST_DEBUG
            if(!boardCompare(&evalTableBoards[key % evalTableSize], & nextBoard)) { 
                printf("WTF! %ld %ld\n", evalTableKey[key % evalTableSize], key);
                drawArray(nextBoard);
                drawArray(evalTableBoards[key % evalTableSize]);
                printf("Recalc: %ld %ld\n", zobrist(& evalTableBoards[key % evalTableSize], WhiteToMove), key);
            }
            #endif
        }
        else {
            #ifdef ZOBRIST_DEBUG
            evalTableBoards[key % evalTableSize] = nextBoard;
            #endif
            eval = -negaMax( & nextBoard, !WhiteToMove, depth - 1, -beta, -alpha);
            evalTable[key % evalTableSize] = eval;
            evalTableKey[key % evalTableSize] = key;
        }
        
        
        

        if (eval >= beta) {
            return beta;
        }
        if (eval > alpha) {
            alpha = eval;
        }
        
        /*
        if(depth == 5) {
        printf("---- %d %d %d %d - %ld - %lf\n", moves.moves[i].startRank, moves.moves[i].startFile, moves.moves[i].endRank, moves.moves[i].endFile, count, eval);
        }
        */

    }
    
    if(validMoves == 0) {
        return 0;
    }
    return alpha;

}

double negaMaxCaptures(struct Board * board, bool WhiteToMove, double alpha, double beta) {

    if(checkCheckmate(board, WhiteToMove)) {
        return -99999.0;
    }
    
    struct MoveList moves;
    struct Board nextBoard;
    
    double eval = evaluate(board, WhiteToMove);
    if(eval >= beta) {
        return beta;
    }
    if (eval > alpha) {
        alpha = eval;
    }

    moves.size = 0;
    if (WhiteToMove) {
        possibleMovesWhite( & moves, board);
    } else {
        possibleMovesBlack( & moves, board);
    }
    
    bubble(board, & moves, WhiteToMove);
    
    int validMoves = 0;
    int64_t attack;
    int64_t opponent;
    bool doFullAnalysis;
    for (int i = 0; i < moves.size; i++) {
        attack = 1LL << (moves.moves[i].endRank * 8 + moves.moves[i].endFile);
        doFullAnalysis = false;
        if(WhiteToMove) {
            opponent = board->blackPawns | board->blackKnights | board->blackBishops | board->blackRooks | board->blackQueens;
            if(attack & opponent) {
                doFullAnalysis = true;
            }
        }
        else {
            opponent = board->whitePawns | board->whiteKnights | board->whiteBishops | board->whiteRooks | board->whiteQueens;
            if(attack & opponent) {
                doFullAnalysis = true;
            }
        }
        
        count++;
        applyMove( board, &nextBoard, moves.moves[i] );
        
        if (WhiteToMove && (nextBoard.whiteKing & blackAttacks(nextBoard.whitePawns, nextBoard.whiteKnights, nextBoard.whiteBishops, nextBoard.whiteRooks, nextBoard.whiteQueens, nextBoard.whiteKing, nextBoard.blackPawns, nextBoard.blackKnights, nextBoard.blackBishops, nextBoard.blackRooks, nextBoard.blackQueens, nextBoard.blackKing))) {
            continue;
            //eval = evaluate( & nextBoard, WhiteToMove);
        }
        else if (!WhiteToMove && (nextBoard.blackKing & whiteAttacks(nextBoard.whitePawns, nextBoard.whiteKnights, nextBoard.whiteBishops, nextBoard.whiteRooks, nextBoard.whiteQueens, nextBoard.whiteKing, nextBoard.blackPawns, nextBoard.blackKnights, nextBoard.blackBishops, nextBoard.blackRooks, nextBoard.blackQueens, nextBoard.blackKing))) {
            continue;
            //eval = evaluate( & nextBoard, WhiteToMove);
        }
        
        validMoves++;
        
        uint64_t key = zobrist(& nextBoard, WhiteToMove);
        
        if(doFullAnalysis) {
            if(evalTableKey[key % evalTableSize] == key) {
                eval = evalTable[key % evalTableSize];
                if(eval > 1000.0) {
                    eval = -99999.0;
                }
                #ifdef ZOBRIST_DEBUG
                if(!boardCompare(&evalTableBoards[key % evalTableSize], & nextBoard)) { 
                    printf("WTF! %ld %ld\n", evalTableKey[key % evalTableSize], key);
                    drawArray(nextBoard);
                    drawArray(evalTableBoards[key % evalTableSize]);
                    printf("Recalc: %ld %ld\n", zobrist(& evalTableBoards[key % evalTableSize], WhiteToMove), key);
                }
                #endif
            }
            else {
                eval = -negaMaxCaptures( & nextBoard, !WhiteToMove, -beta, -alpha);
            }
        }
        else {
            eval = evaluate(board, WhiteToMove);
        }
        #ifdef ZOBRIST_DEBUG
        evalTableBoards[key % evalTableSize] = nextBoard;
        #endif
        evalTable[key % evalTableSize] = eval;
        evalTableKey[key % evalTableSize] = key;

        if (eval >= beta) {
            return beta;
        }
        if (eval > alpha) {
            alpha = eval;
        }

    }
    if(validMoves == 0) {
        return 0;
    }    
    return alpha;

}

double evaluate(struct Board * board, bool WhiteToMove) {
    int whitePawns = __builtin_popcountll(board -> whitePawns);
    int whiteKnights = __builtin_popcountll(board -> whiteKnights);
    int whiteBishops = __builtin_popcountll(board -> whiteBishops);
    int whiteRooks = __builtin_popcountll(board -> whiteRooks);
    int whiteQueens = __builtin_popcountll(board -> whiteQueens);

    int whiteCenteredPieces = __builtin_popcountll((board -> whitePawns | board -> whiteKnights | board -> whiteBishops) & 0x00007E7E7E7E0000LL);

    int blackPawns = __builtin_popcountll(board -> blackPawns);
    int blackKnights = __builtin_popcountll(board -> blackKnights);
    int blackBishops = __builtin_popcountll(board -> blackBishops);
    int blackRooks = __builtin_popcountll(board -> blackRooks);
    int blackQueens = __builtin_popcountll(board -> blackQueens);

    int blackCenteredPieces = __builtin_popcountll((board -> blackPawns | board -> blackKnights | board -> blackBishops) & 0x00007E7E7E7E0000LL);

    double eval = whitePawns * 1.0 + whiteKnights * 2.5 + whiteBishops * 3.0 + whiteRooks * 5.0 + whiteQueens * 9.0 + whiteCenteredPieces * 0.1;
    eval -= blackPawns * 1.0 + blackKnights * 2.5 + blackBishops * 3.0 + blackRooks * 5.0 + blackQueens * 9.0 + blackCenteredPieces * 0.1;

    if(checkCheckmate(board, WhiteToMove)) {
        if(WhiteToMove) {
            eval -= 9999.0;
        }
        else{
            eval += 9999.0;
        }
    }
    
    int wkX = __builtin_ctzll(board->whiteKing) / 8;
    int wkY = __builtin_ctzll(board->whiteKing) % 8;
    int bkX = __builtin_ctzll(board->blackKing) / 8;
    int bkY = __builtin_ctzll(board->blackKing) % 8;
    
    int kingDistance = abs(wkX - bkX) + abs(wkY - bkY);
    
    double kingDistanceFactor = (16.0 - kingDistance) / 1000.0 + 1.0;
    
    eval = eval * kingDistanceFactor;

    return WhiteToMove ? eval : -eval;
}

void prune(struct Board * board, struct MoveList * moves, bool WhiteToMove) {
    int64_t attacks = WhiteToMove ? blackAttacks(board->whitePawns, board->whiteKnights, board->whiteBishops, board->whiteRooks, board->whiteQueens, board->whiteKing, board->blackPawns, board->blackKnights, board->blackBishops, board->blackRooks, board->blackQueens, board->blackKing) : whiteAttacks(board->whitePawns, board->whiteKnights, board->whiteBishops, board->whiteRooks, board->whiteQueens, board->whiteKing, board->blackPawns, board->blackKnights, board->blackBishops, board->blackRooks, board->blackQueens, board->blackKing);
    int64_t defends = !WhiteToMove ? blackAttacks(board->whitePawns, board->whiteKnights, board->whiteBishops, board->whiteRooks, board->whiteQueens, board->whiteKing, board->blackPawns, board->blackKnights, board->blackBishops, board->blackRooks, board->blackQueens, board->blackKing) : whiteAttacks(board->whitePawns, board->whiteKnights, board->whiteBishops, board->whiteRooks, board->whiteQueens, board->whiteKing, board->blackPawns, board->blackKnights, board->blackBishops, board->blackRooks, board->blackQueens, board->blackKing);
    int64_t to;
    int64_t from;
    int64_t pawns = board->whitePawns | board->blackPawns;
    for (int i = 0; i < moves->size; i++) {
        to = 1LL << (moves -> moves[i].endRank * 8 + moves -> moves[i].endFile);
        if(to & attacks & ~defends) {
            from = 1LL << (moves -> moves[i].startRank * 8 + moves -> moves[i].startFile);
            if(~(from & pawns)) {
                moves->size--;
                moves -> moves[i] = moves -> moves[moves->size];
            }
        }
    }
}

void bubble(struct Board * board, struct MoveList * moves, bool WhiteToMove) {
    int64_t OPPOSING_PIECES = WhiteToMove ? (board -> blackPawns | board -> blackKnights | board -> blackBishops | board -> blackRooks | board -> blackQueens | board -> blackKing) : (board -> whitePawns | board -> whiteKnights | board -> whiteBishops | board -> whiteRooks | board -> whiteQueens | board -> whiteKing);
    int64_t attack;
    int swapPlace = 0;
    for (int i = 0; i < moves -> size; i++) {
        attack = 1LL << (moves -> moves[i].endRank * 8 + moves -> moves[i].endFile);
        if (attack & OPPOSING_PIECES || moves -> moves[i].endFile == PAWN_PROMOTION) {
            struct Move temp = moves -> moves[swapPlace];
            moves -> moves[swapPlace] = moves -> moves[i];
            moves -> moves[i] = temp;
            swapPlace++;
        }
    }
    
    int64_t attacks = WhiteToMove ? blackAttacks(board->whitePawns, board->whiteKnights, board->whiteBishops, board->whiteRooks, board->whiteQueens, board->whiteKing, board->blackPawns, board->blackKnights, board->blackBishops, board->blackRooks, board->blackQueens, board->blackKing) : whiteAttacks(board->whitePawns, board->whiteKnights, board->whiteBishops, board->whiteRooks, board->whiteQueens, board->whiteKing, board->blackPawns, board->blackKnights, board->blackBishops, board->blackRooks, board->blackQueens, board->blackKing);
    
    int64_t aMove;
    int swapPlace2 = moves -> size - 1;
    for (int i = swapPlace; i < moves -> size; i++) {
        aMove = 1LL << (moves -> moves[i].endRank * 8 + moves -> moves[i].endFile);
        if (attack & aMove) {
            struct Move temp = moves -> moves[swapPlace];
            moves -> moves[swapPlace] = moves -> moves[i];
            moves -> moves[i] = temp;
            swapPlace2--;
        }
    }
}

int compareMoves (const void * a, const void * b) {
    if(((struct MoveSortable*)a)->value > ((struct MoveSortable*)b)->value) {
        return -1;
    }
    return 1;
}
    
void bubbleAdvanced(struct Board * board, struct MoveList * moves, bool WhiteToMove) {
    int64_t attacks = WhiteToMove ? blackAttacks(board->whitePawns, board->whiteKnights, board->whiteBishops, board->whiteRooks, board->whiteQueens, board->whiteKing, board->blackPawns, board->blackKnights, board->blackBishops, board->blackRooks, board->blackQueens, board->blackKing) : whiteAttacks(board->whitePawns, board->whiteKnights, board->whiteBishops, board->whiteRooks, board->whiteQueens, board->whiteKing, board->blackPawns, board->blackKnights, board->blackBishops, board->blackRooks, board->blackQueens, board->blackKing);
    struct MoveSortable *movesSorted = malloc(moves->size * sizeof(struct MoveSortable));
    int64_t attack;
    int64_t from;
    for (int i = 0; i < moves->size; i++) {
        movesSorted[i].move = moves->moves[i];
        movesSorted[i].value = 0.0;
        if(moves->moves[i].endFile == PAWN_PROMOTION) {
            movesSorted[i].value = 9.0;
            continue;
        }
        attack = 1LL << (moves -> moves[i].endRank * 8 + moves -> moves[i].endFile);
        from = 1LL << (moves -> moves[i].startRank * 8 + moves -> moves[i].startFile);
        if(WhiteToMove) {
            if(attack & board->blackPawns) {
                movesSorted[i].value = 1.0;
                continue;
            }
            else if(attack & board->blackKnights) {
                movesSorted[i].value = 2.5;
                continue;
            }
            else if(attack & board->blackBishops) {
                movesSorted[i].value = 3.0;
                continue;
            }
            else if(attack & board->blackRooks) {
                movesSorted[i].value = 5.0;
                continue;
            }
            else if(attack & board->blackQueens) {
                movesSorted[i].value = 9.0;
                continue;
            }
            else if(attack & board->blackKing) {
                movesSorted[i].value = 30.0;
                continue;
            }
            
            if((from & board->whitePawns) && (attack & attacks)) {
                movesSorted[i].value = -1.0;
                continue;
            }
            else if((from & board->whiteKnights) && (attack & attacks)) {
                movesSorted[i].value = -2.5;
                continue;
            }
            else if((from & board->whiteBishops) && (attack & attacks)) {
                movesSorted[i].value = -3.0;
                continue;
            }
            else if((from & board->whiteRooks) && (attack & attacks)) {
                movesSorted[i].value = -5.0;
                continue;
            }
            else if((from & board->whiteQueens) && (attack & attacks)) {
                movesSorted[i].value = -9.0;
                continue;
            }
        }
        else {
            if(attack & board->whitePawns) {
                movesSorted[i].value = 1.0;
                continue;
            }
            else if(attack & board->whiteKnights) {
                movesSorted[i].value = 2.5;
                continue;
            }
            else if(attack & board->whiteBishops) {
                movesSorted[i].value = 3.0;
                continue;
            }
            else if(attack & board->whiteRooks) {
                movesSorted[i].value = 5.0;
                continue;
            }
            else if(attack & board->whiteQueens) {
                movesSorted[i].value = 9.0;
                continue;
            }
            else if(attack & board->whiteKing) {
                movesSorted[i].value = 30.0;
                continue;
            }
            
            if(from & board->blackPawns && attack & attacks) {
                movesSorted[i].value = -1.0;
                continue;
            }
            else if(from & board->blackKnights && attack & attacks) {
                movesSorted[i].value = -2.5;
                continue;
            }
            else if(from & board->blackBishops && attack & attacks) {
                movesSorted[i].value = -3.0;
                continue;
            }
            else if(from & board->blackRooks && attack & attacks) {
                movesSorted[i].value = -5.0;
                continue;
            }
            else if(from & board->blackQueens && attack & attacks) {
                movesSorted[i].value = -9.0;
                continue;
            }
        }
    }
    
    qsort(movesSorted, moves->size, sizeof(struct MoveSortable), compareMoves);
    
    for (int i = 0; i < moves->size; i++) {
        moves->moves[i] = movesSorted[i].move;
    }
    
    free(movesSorted);
}

//fuck you buddy
uint64_t mcg64(void) {
    static uint64_t i = 1;
    i = (682468683931853921LL * i) % 8883904893065660923LL;
    return i;
}

uint64_t rnd64(uint64_t n)
{
    const uint64_t z = 0x9FB21C651E98DF25;

    n ^= ((n << 49) | (n >> 15)) ^ ((n << 24) | (n >> 40));
    n *= z;
    n ^= n >> 35;
    n *= z;
    n ^= n >> 28;

    return n;
}

void genZobrist(void) {
    for(int64_t i = 0; i < evalTableSize; i++) {
        evalTableKey[i] = 0;
        evalTableBoards[i].whitePawns = 0;
    }
    uint64_t seed = 1;
    
    for(int color = 0; color < 2; color++) {
        for(int piece = 0; piece < 6; piece++) {
            for(int square = 0; square < 64; square++) {
                zArray[color][piece][square] = rnd64(seed++);
            }
        }
    }
    
    for(int i = 0; i < 8; i++) {
        zEnPassant[i] = rnd64(seed++);
    }
    
    for(int i = 0; i < 4; i++) {
        zCastle[i] = rnd64(seed++);
    }
    
    for(int i = 0; i < 4; i++) {
        zBlackMove = rnd64(seed++);
    }
}


uint64_t zobrist(struct Board * board, bool WhiteToMove) {
    uint64_t key = 0LL;
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

    int64_t WHITE_PIECES = (wp | wn | wb | wr | wq | wk);
    int64_t BLACK_PIECES = (bp | bn | bb | br | bq | bk);
    
    int64_t OCCUPIED = WHITE_PIECES | BLACK_PIECES;
    for(int i = 0; i < 64; i++) {
    
        if(~OCCUPIED & (1LL << i)) {
            continue;
        }
        
        if(wp & (1LL << i)) {
            key ^= zArray[0][0][i];
        }
        else if(bp & (1LL << i)) {
            key ^= zArray[1][0][i];
        }
        else if(wn & (1LL << i)) {
            key ^= zArray[0][1][i];
        }
        else if(bn & (1LL << i)) {
            key ^= zArray[1][1][i];
        }
        else if(wb & (1LL << i)) {
            key ^= zArray[0][2][i];
        }
        else if(bb & (1LL << i)) {
            key ^= zArray[1][2][i];
        }
        else if(wr & (1LL << i)) {
            key ^= zArray[0][3][i];
        }
        else if(br & (1LL << i)) {
            key ^= zArray[1][3][i];
        }
        else if(wq & (1LL << i)) {
            key ^= zArray[0][4][i];
        }
        else if(bq & (1LL << i)) {
            key ^= zArray[1][4][i];
        }
        else if(wk & (1LL << i)) {
            key ^= zArray[0][5][i];
        }
        else if(bk & (1LL << i)) {
            key ^= zArray[1][5][i];
        }
    }
    
    for(int i = 0; i < 8; i++) {
        if(ep & FILE_MASKS[i]) {
            key ^= zEnPassant[i];
        }
    }
    
    if(board->whiteKingKingsideCastle) {
        key ^= zCastle[0];
    }
    if(board->whiteKingQueensideCastle) {
        key ^= zCastle[1];
    }
    if(board->blackKingKingsideCastle) {
        key ^= zCastle[2];
    }
    if(board->blackKingQueensideCastle) {
        key ^= zCastle[3];
    }
    
    if(!WhiteToMove) {
        key ^= zBlackMove;
    }
    
    return key;
}
