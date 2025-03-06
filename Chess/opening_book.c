#include <stdio.h>

#include <string.h>

#include <stdlib.h>

#include "opening_book.h"

#include "boardbuilder.h"

FILE *fr;

//#define DEBUG_MODE
struct OpeningNode firstNode;

bool equalMoves(struct Move move1, struct Move move2) {
    if(move1.startRank != move2.startRank) {
        return false;
    }
    if(move1.startFile != move2.startFile) {
        return false;
    }
    if(move1.endRank != move2.endRank) {
        return false;
    }
    if(move1.endFile != move2.endFile) {
        return false;
    }
    return true;
}

void generateOpeningBook(void) {
    firstNode.move.startFile = 0;
    firstNode.move.startRank = 0;
    firstNode.move.endFile = 0;
    firstNode.move.endRank = 0;
    firstNode.weight = 0;
    firstNode.responseWeightSum = 0;
    firstNode.responseCount = 0;
    struct Board board;
    zeroBitboards( & board);
    buildStandardChess( & board);
    struct OpeningNode * currentLineNode = & firstNode;
    fr = fopen("debuglines.pgn", "r");
    char whitesMovePGN[10];
    char blacksMovePGN[10];
    int moveNumber;
    int valuesFound = 3;
    struct Board nextBoard;
    zeroBitboards( & nextBoard);
    struct Move whitesMove;
    struct Move blacksMove;
    bool foundChild;
    while (1) {
        valuesFound = fscanf(fr, "%d %9s %9s", & moveNumber, whitesMovePGN, blacksMovePGN);
        if (valuesFound != 3) {
            break;
        }
        if (moveNumber == 1) {
            currentLineNode = & firstNode;
            zeroBitboards( & board);
            buildStandardChess( & board);
        }
        //printf("Move for white: '%s' move for black '%s'\n", whitesMovePGN, blacksMovePGN);
        whitesMove = pgnToMove( & board, whitesMovePGN, true);
        //printf("White translation: %d %d %d% d\n", whitesMove.startRank, whitesMove.startFile, whitesMove.endRank, whitesMove.endFile);


        foundChild = false;
        for (int l = 0; l < currentLineNode -> responseCount; l++) {
            //printf("silly buisness\n");
            if (equalMoves(whitesMove, currentLineNode -> responses[l] -> move)) {
                currentLineNode -> responseWeightSum++;
                currentLineNode -> responses[l] -> weight++;
                currentLineNode = currentLineNode -> responses[l];
                foundChild = true;
            }
        }
        if (!foundChild) {
            //printf("silly buisness\n");
            if (currentLineNode -> responseCount < MAX_RESPONSES) {
                currentLineNode -> responses[currentLineNode -> responseCount] = malloc(sizeof(struct OpeningNode));
                currentLineNode -> responseCount++;
                currentLineNode -> responseWeightSum++;
                currentLineNode = currentLineNode -> responses[currentLineNode -> responseCount - 1];
                currentLineNode -> move.startFile = whitesMove.startFile;
                currentLineNode -> move.startRank = whitesMove.startRank;
                currentLineNode -> move.endFile = whitesMove.endFile;
                currentLineNode -> move.endRank = whitesMove.endRank;
                currentLineNode -> weight = 1;
                currentLineNode -> responseWeightSum = 0;
                currentLineNode -> responseCount = 0;
            } else {
                while (moveNumber < 8) {
                    valuesFound = fscanf(fr, "%d %9s %9s", & moveNumber, whitesMovePGN, blacksMovePGN);
                }
                continue;
            }
        }
        

        applyMove( &board, &nextBoard, whitesMove );
        
        board = nextBoard;
        zeroBitboards( & nextBoard);

        blacksMove = pgnToMove( & board, blacksMovePGN, false);
        
        //printf("Black translation: %d %d %d% d\n", blacksMove.startRank, blacksMove.startFile, blacksMove.endRank, blacksMove.endFile);
        
        
        foundChild = false;
        for (int l = 0; l < currentLineNode -> responseCount; l++) {
            if (equalMoves(blacksMove, currentLineNode -> responses[l] -> move)) {
                currentLineNode -> responseWeightSum++;
                currentLineNode -> responses[l] -> weight++;
                currentLineNode = currentLineNode -> responses[l];
                foundChild = true;
            }
        }
        if (!foundChild) {
            if (currentLineNode -> responseCount < MAX_RESPONSES) {
                currentLineNode -> responses[currentLineNode -> responseCount] = malloc(sizeof(struct OpeningNode));
                currentLineNode -> responseCount++;
                currentLineNode -> responseWeightSum++;
                currentLineNode = currentLineNode -> responses[currentLineNode -> responseCount - 1];
                currentLineNode -> move.startFile = blacksMove.startFile;
                currentLineNode -> move.startRank = blacksMove.startRank;
                currentLineNode -> move.endFile = blacksMove.endFile;
                currentLineNode -> move.endRank = blacksMove.endRank;
                currentLineNode -> weight = 1;
                currentLineNode -> responseWeightSum = 0;
                currentLineNode -> responseCount = 0;
            } else {
                while (moveNumber < 8) {
                    valuesFound = fscanf(fr, "%d %9s %9s", & moveNumber, whitesMovePGN, blacksMovePGN);
                }
                continue;
            }
        }
        

        applyMove( &board, &nextBoard, blacksMove );
        
        board = nextBoard;
        zeroBitboards( & nextBoard);
    }
}

struct Move pgnToMove(struct Board * board, char * s, bool WhiteToMove) {
    for(int i = 0; i < strlen(s); i++) {
        if(s[i] == 'x') {
            for(int j = i; j < strlen(s)-1; j++) {
               s[j] = s[j+1];
            }
            s[strlen(s) - 1] = '\0';
            break;
        }
    }
    #ifdef DEBUG_MODE
        printf("Post x removal: %s\n", s);
    #endif
    struct MoveList moves;
    moves.size = 0;
    
    struct Move move;
    
    if (WhiteToMove) {
        possibleMovesWhite( & moves, board);
    } else {
        possibleMovesBlack( & moves, board);
    }
    
    if(!strcmp(s, "O-O")) {
        #ifdef DEBUG_MODE
            printf("Kingside castle: %s\n", s);
        #endif
        if(WhiteToMove) {
            move.startRank = 7;
            move.startFile = 4;
            move.endRank = 7;
            move.endFile = 6;
        }
        else {
            move.startRank = 0;
            move.startFile = 4;
            move.endRank = 0;
            move.endFile = 6;
        }
        return move;
    }
    else if(!strcmp(s, "O-O-O")) {
        #ifdef DEBUG_MODE
            printf("Queenside castle: %s\n", s);
        #endif
        if(WhiteToMove) {
            move.startRank = 7;
            move.startFile = 4;
            move.endRank = 7;
            move.endFile = 2;
        }
        else {
            move.startRank = 0;
            move.startFile = 4;
            move.endRank = 0;
            move.endFile = 2;
        }
        return move;
    }
    
    int i = strlen(s) - 1;
    while(s[i] < 'a' | s[i] > 'h') {
        i--;
    }
    #ifdef DEBUG_MODE
    printf("End location in string: %d\n", i);
    #endif
    move.endFile = s[i] - 'a';
    move.endRank = 7 - (s[i+1] - '1');
    #ifdef DEBUG_MODE
        printf("First Estimate: End file: %d End rank: %d\n", move.endFile , move.endRank);
    #endif
    if(i) {
        if(i == 1) {
            int64_t fromPieces;
            bool isPawnMove = false;;
            switch(s[i - 1]) {
                case 'N':
                    #ifdef DEBUG_MODE
                        printf("Examining knight moves\n");
                    #endif
                    fromPieces = WhiteToMove ? board->whiteKnights : board->blackKnights;
                    break;
                case 'B':
                    #ifdef DEBUG_MODE
                        printf("Examining bishop moves\n");
                    #endif
                    fromPieces = WhiteToMove ? board->whiteBishops : board->blackBishops;
                    break;
                case 'R':
                    #ifdef DEBUG_MODE
                        printf("Examining rook moves\n");
                    #endif
                    fromPieces = WhiteToMove ? board->whiteRooks : board->blackRooks;
                    break;
                case 'Q':
                    #ifdef DEBUG_MODE
                        printf("Examining queen moves\n");
                    #endif
                    fromPieces = WhiteToMove ? board->whiteQueens : board->blackQueens;
                    break;
                case 'K':
                    #ifdef DEBUG_MODE
                        printf("Examining king moves\n");
                    #endif
                    fromPieces = WhiteToMove ? board->whiteKing : board->blackKing;
                    break;
                default:
                    #ifdef DEBUG_MODE
                        printf("Examining pawn moves\n");
                    #endif
                    isPawnMove = true;
                    move.startFile = s[i-1] - 'a';
                    fromPieces = WhiteToMove ? board->whitePawns : board->blackPawns;
            }
            for (int j = 0; j < moves.size; j++) { 
                int64_t from = moves.moves[j].startRank * 8 + moves.moves[j].startFile;
                #ifdef DEBUG_MODE
                    printf("Examining move starting at Rank: %d File %d\n--Ending at: Rank: %d File %d\n", moves.moves[j].startRank, moves.moves[j].startFile, moves.moves[j].endRank, moves.moves[j].endFile);
                #endif
                if(((1LL << from) & fromPieces)) {
                    if(move.endFile == moves.moves[j].endFile) {
                        if((move.endRank == moves.moves[j].endRank)) {
                            if(!isPawnMove || move.startFile == moves.moves[j].startFile) {
                                move.startRank = moves.moves[j].startRank;
                                move.startFile = moves.moves[j].startFile;
                            }
                        }
                        else{
                            #ifdef DEBUG_MODE
                                printf("----Failed due to wrong end rank\n");
                            #endif
                        }
                    }
                    else{
                        #ifdef DEBUG_MODE
                            printf("----Failed due to wrong end file\n");
                        #endif
                    }
                }
                else{
                    #ifdef DEBUG_MODE
                        printf("----Failed due to wrong piece\n");
                    #endif
                }
                if(moves.moves[j].endFile == 11 && moves.moves[j].startRank == move.startFile && ((s[strlen(s) - 2] == '=') || (s[strlen(s) - 3] == '='))) {
                    move.startRank = moves.moves[j].startRank;
                    move.startFile = moves.moves[j].startFile;
                    move.endFile = 11;
                }
            }
        }
        else if (i == 2) {
            if(s[i-2] > 'a' && s[i-2] < 'h') {
                move.startFile = s[i-2] - 'a';
                move.startRank = 7 - (s[i-1] - '1');
                #ifdef DEBUG_MODE
                    printf("Found start: Rank: %d File: %d\n",move.startRank, move.startFile);
                #endif
            }
            else{
                move.startFile = s[i-1] - 'a';
                int64_t fromPieces;
                switch(s[i - 2]) {
                    case 'N':
                        fromPieces = WhiteToMove ? board->whiteKnights : board->blackKnights;
                        break;
                    case 'B':
                        fromPieces = WhiteToMove ? board->whiteBishops : board->blackBishops;
                        break;
                    case 'R':
                        fromPieces = WhiteToMove ? board->whiteRooks : board->blackRooks;
                        break;
                    case 'Q':
                        fromPieces = WhiteToMove ? board->whiteQueens : board->blackQueens;
                        break;
                    case 'K':
                        fromPieces = WhiteToMove ? board->whiteKing : board->blackKing;
                        break;
                    default:
                        printf("Illegal move....\n");
                }
                for (int j = 0; j < moves.size; j++) { 
                    int64_t from = moves.moves[j].startRank * 8 + move.startFile;
                    if((1LL << from) & fromPieces) {
                        if (move.endFile == moves.moves[j].endFile) {
                            if (move.endRank == moves.moves[j].endRank) {
                                if(move.startFile == moves.moves[j].startFile) {
                                    return moves.moves[j];
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else {
        int64_t fromPieces = WhiteToMove ? board->whitePawns : board->blackPawns;
        move.startFile = s[i] - 'a';
        move.startRank = 7 - (s[i+1] - '1');
        for (int j = 0; j < moves.size; j++) { 
            #ifdef DEBUG_MODE
                printf("Examining move starting at Rank: %d File %d\n--Ending at: Rank: %d File %d\n", moves.moves[j].startRank, moves.moves[j].startFile, moves.moves[j].endRank, moves.moves[j].endFile);
            #endif
            int64_t from = moves.moves[j].startRank * 8 + moves.moves[j].startFile;
            if(((1LL << from) & fromPieces)) {
                if(move.endFile == moves.moves[j].endFile) {
                    if((move.endRank == moves.moves[j].endRank)) {
                        move.startRank = moves.moves[j].startRank;
                        move.startFile = moves.moves[j].startFile;
                    }
                    else{
                        #ifdef DEBUG_MODE
                            printf("----Failed due to wrong end rank\n");
                        #endif
                    }
                }
                else{
                    #ifdef DEBUG_MODE
                        printf("----Failed due to wrong end file\n");
                    #endif
                }
            }
            else{
                    #ifdef DEBUG_MODE
                        printf("----Failed due to wrong piece\n");
                    #endif
            }
            if(moves.moves[j].endFile == 11 && moves.moves[j].startRank == move.startFile && ((s[strlen(s) - 2] == '=') || (s[strlen(s) - 3] == '='))) {
                move.startRank = moves.moves[j].startRank;
                move.startFile = moves.moves[j].startFile;
                move.endFile = 11;
                #ifdef DEBUG_MODE
                printf("----Valid!\n");
                #endif
            }
        }
    }
    
    if(s[strlen(s) - 2] == '=') {
        switch(s[strlen(s) - 1]) {
            case 'N':
                move.endRank = WhiteToMove ? WHITE_KNIGHT : BLACK_KNIGHT;
                break;
            case 'B':
                move.endRank = WhiteToMove ? WHITE_BISHOP : BLACK_BISHOP;
                break;
            case 'R':
                move.endRank = WhiteToMove ? WHITE_ROOK : BLACK_ROOK;
                break;
            case 'Q':
                move.endRank = WhiteToMove ? WHITE_QUEEN : BLACK_QUEEN;
                break;
            default:
                printf("Illegal move....\n");
        }
    }
    else if(strlen(s) >= 3 && s[strlen(s) - 3] == '=') {
        switch(s[strlen(s) - 2]) {
            case 'N':
                move.endRank = WhiteToMove ? WHITE_KNIGHT : BLACK_KNIGHT;
                break;
            case 'B':
                move.endRank = WhiteToMove ? WHITE_BISHOP : BLACK_BISHOP;
                break;
            case 'R':
                move.endRank = WhiteToMove ? WHITE_ROOK : BLACK_ROOK;
                break;
            case 'Q':
                move.endRank = WhiteToMove ? WHITE_QUEEN : BLACK_QUEEN;
                break;
            default:
                printf("Illegal move....\n");
        }
    }
    return move;
}
