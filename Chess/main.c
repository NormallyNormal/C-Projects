#include <stdio.h>

#include <stdlib.h>

#include <math.h>

#include <time.h>

#include "moves.h"

#include "board.h"

#include "boardbuilder.h"

#include "perft.h"

#include "ai.h"

#include "opening_book.h"

//#define TWO_PARTY
#define USE_BOOK
extern struct OpeningNode firstNode;

int main() {
    init();
    bool inBook = true;
    time_t t;
    srand((unsigned) time(&t));
    bool WhiteToMove = true;
    struct Board board;
    zeroBitboards( & board);
    buildStandardChess( & board);
    drawArrayNumbered(board);
    int playerMoveIn;
    char playerMovePGN[10];
    char promote;
    struct Move playerMove;
    
    #ifdef USE_BOOK
    generateOpeningBook();
    #endif
    
    while (1) {
        int shutUp = scanf("%9s", playerMovePGN);
        playerMove = pgnToMove(& board, playerMovePGN, WhiteToMove);
        inBook = false;
        for(int i = 0; i < firstNode.responseCount; i++) {
            if(equalMoves(playerMove, firstNode.responses[i]->move)) {
                firstNode = *firstNode.responses[i];
                inBook = true;
            }
        }
        printf("%d %d %d %d\n", playerMove.startRank, playerMove.startFile, playerMove.endRank, playerMove.endFile);
        /*
        promote = '\n';
        int shutup = scanf("%4d%c", & playerMoveIn, & promote);
        if (shutup = 0) {
            continue;
        }
        playerMove.startRank = playerMoveIn / 1000;
        playerMoveIn = playerMoveIn % 1000;
        playerMove.startFile = playerMoveIn / 100;
        playerMoveIn = playerMoveIn % 100;
        playerMove.endRank = playerMoveIn / 10;
        playerMoveIn = playerMoveIn % 10;
        playerMove.endFile = playerMoveIn;
        
        printf("%d %d %d% d\n", playerMove.startRank, playerMove.startFile, playerMove.endRank, playerMove.endFile);
        
        if(promote != '\n') {
            playerMove.startRank = playerMove.startFile;
            playerMove.startFile = playerMove.endFile;
            playerMove.endFile = 11;
            switch(promote) {
                case 'N':
                    playerMove.endRank = WHITE_KNIGHT;
                    break;
                case 'B':
                    playerMove.endRank = WHITE_BISHOP;
                    break;
                case 'R':
                    playerMove.endRank = WHITE_ROOK;
                    break;
                case 'Q':
                    playerMove.endRank = WHITE_QUEEN;
                    break;
                default:
                    printf("Illegal move....\n");
                    continue;
            }
        }
        */
        
        

        struct MoveList moves;
        struct Board nextBoard;
        zeroBitboards( & nextBoard);
        moves.size = 0;
        if (WhiteToMove) {
            possibleMovesWhite( & moves, & board);
        } else {
            possibleMovesBlack( & moves, & board);
        }

        bool isLegal = false;
        for (int i = 0; i < moves.size; i++) {
            if (moves.moves[i].startRank == playerMove.startRank && moves.moves[i].startFile == playerMove.startFile && moves.moves[i].endRank == playerMove.endRank && moves.moves[i].endFile == playerMove.endFile) {
                isLegal = true;
            }
        }
        if (!isLegal) {
            printf("Illegal move...\n");
            continue;
        }

        applyMove( &board, &nextBoard, playerMove );

        if (WhiteToMove && (nextBoard.whiteKing & blackAttacks(nextBoard.whitePawns, nextBoard.whiteKnights, nextBoard.whiteBishops, nextBoard.whiteRooks, nextBoard.whiteQueens, nextBoard.whiteKing, nextBoard.blackPawns, nextBoard.blackKnights, nextBoard.blackBishops, nextBoard.blackRooks, nextBoard.blackQueens, nextBoard.blackKing))) {
            printf("Illegal move...\n");
            continue;
        }

        board = nextBoard;
        zeroBitboards( & nextBoard);

        WhiteToMove = !WhiteToMove;

        drawArrayNumbered(board);

        #ifndef TWO_PARTY
        if(inBook && firstNode.responseCount > 0 && firstNode.responseWeightSum > 0) {
            int rngMove = rand();
            rngMove = rngMove % firstNode.responseWeightSum;
            int rngMoveChoose = 0;
            while(rngMove > firstNode.responses[rngMoveChoose]->weight) {
                rngMove -= firstNode.responses[rngMoveChoose]->weight;
                rngMoveChoose++;
            }
            playerMove = firstNode.responses[rngMoveChoose]->move;
            firstNode = *firstNode.responses[rngMoveChoose];
            printf("Book\n");
        }
        else{
            firstNode.responseCount = 0;
            printf("Thinking");
            fflush(stdout);
            struct Move * botMove = findBest( & board, WhiteToMove, 6);
            playerMove = * botMove;
            free(botMove);
        }
        #endif
        
        #ifdef TWO_PARTY
        shutUp = scanf("%9s", playerMovePGN);
        playerMove = pgnToMove(& board, playerMovePGN, WhiteToMove);
        #endif
        
        printf("%d %d %d% d\n", playerMove.startRank, playerMove.startFile, playerMove.endRank, playerMove.endFile);

        applyMove( &board, &nextBoard, playerMove );

        board = nextBoard;

        WhiteToMove = !WhiteToMove;

        drawArrayNumbered(board);
    }
    return 0;
}
