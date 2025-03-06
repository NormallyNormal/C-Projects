#include <stdio.h>

#include "board.h"
#include "boardbuilder.h"

char oldChessBoard[8][8];

void buildStandardChess(struct Board *board) {

    char charBoard[8][8] = {
            'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
            'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
            'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'
    };

/*
    char charBoard[8][8] = {
            'r', 'n', 'b', ' ', 'k', 'b', 'n', 'r',
            'p', 'p', 'p', 'p', ' ', 'p', 'p', 'p',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', 'p', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', 'P', ' ', ' ', 'q',
            ' ', ' ', ' ', 'P', ' ', ' ', ' ', ' ',
            'P', 'P', 'P', ' ', ' ', 'P', 'P', 'P',
            'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'
    };
*/
/*
    char charBoard[8][8] = {
            'r', ' ', 'b', 'q', 'k', 'b', ' ', 'r',
            ' ', 'p', ' ', 'p', ' ', 'p', ' ', 'p',
            ' ', ' ', ' ', ' ', ' ', ' ', 'p', ' ',
            'p', ' ', ' ', ' ', 'p', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', 'N', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', 'B', 'N', ' ', ' ',
            ' ', 'P', 'P', ' ', 'Q', 'P', 'P', 'P',
            'K', ' ', ' ', 'R', ' ', 'B', ' ', 'R'
    };
*/
/*
    char charBoard[8][8] = {
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '
    };
*/
/*
    char charBoard[8][8] = {
            'r', ' ', 'b', ' ', 'k', 'b', ' ', 'r',
            'p', ' ', ' ', ' ', ' ', 'p', ' ', 'p',
            ' ', ' ', 'p', ' ', 'p', 'n', 'p', ' ',
            ' ', ' ', ' ', 'q', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ',
            'P', 'P', 'P', ' ', ' ', 'P', 'P', 'P',
            'R', 'N', 'B', 'Q', 'R', ' ', 'K', ' '
    };
*/
    chessBoardToBitboards(charBoard, board);
}

void chessBoardToBitboards(char chessBoard[8][8], struct Board *board) {
    int64_t bitLocation = 1;
    for(int i = 0; i < 64; i++) {
        switch (chessBoard[i / 8][i % 8]) {
            case 'P':
                board->whitePawns |= bitLocation;
                break;
            case 'N':
                board->whiteKnights |= bitLocation;
                break;
            case 'B':
                board->whiteBishops |= bitLocation;
                break;
            case 'R':
                board->whiteRooks |= bitLocation;
                break;
            case 'Q':
                board->whiteQueens |= bitLocation;
                break;
            case 'K':
                board->whiteKing |= bitLocation;
                break;
            case 'p':
                board->blackPawns |= bitLocation;
                break;
            case 'n':
                board->blackKnights |= bitLocation;
                break;
            case 'b':
                board->blackBishops |= bitLocation;
                break;
            case 'r':
                board->blackRooks |= bitLocation;
                break;
            case 'q':
                board->blackQueens |= bitLocation;
                break;
            case 'k':
                board->blackKing |= bitLocation;
                break;
            default:
                break;
        }
        bitLocation = bitLocation << 1;
    }
    for (int i=0;i<64;i++) {
        oldChessBoard[i/8][i%8] = chessBoard[i/8][i%8];
    }
}

void zeroBitboards(struct Board *board) {
    board->whitePawns = 0;
    board->whiteKnights = 0;
    board->whiteBishops = 0;
    board->whiteRooks = 0;
    board->whiteQueens = 0;
    board->whiteKing = 0;
    board->blackPawns = 0;
    board->blackKnights = 0;
    board->blackBishops = 0;
    board->blackRooks = 0;
    board->blackQueens = 0;
    board->blackKing = 0;
    board->whiteKingQueensideCastle = true;
    board->whiteKingKingsideCastle = true;
    board->blackKingQueensideCastle = true;
    board->blackKingKingsideCastle = true;
}

void drawArray(struct Board board) {
    char chessBoard[8][8];
    for (int i=0;i<64;i++) {
        chessBoard[i/8][i%8]=' ';
    }
    for (int i=0;i<64;i++) {
        if (((board.whitePawns>>i)&1)==1) {
            chessBoard[i/8][i%8]='P';
        }
        if (((board.whiteKnights>>i)&1)==1) {
            chessBoard[i/8][i%8]='N';}
        if (((board.whiteBishops>>i)&1)==1) {chessBoard[i/8][i%8]='B';
        }
        if (((board.whiteRooks>>i)&1)==1) {
            chessBoard[i/8][i%8]='R';
        }
        if (((board.whiteQueens>>i)&1)==1) {
            chessBoard[i/8][i%8]='Q';}
        if (((board.whiteKing>>i)&1)==1) {chessBoard[i/8][i%8]='K';
        }
        if (((board.blackPawns>>i)&1)==1) {
            chessBoard[i/8][i%8]='p';}
        if (((board.blackKnights>>i)&1)==1) {chessBoard[i/8][i%8]='n';
        }
        if (((board.blackBishops>>i)&1)==1) {
            chessBoard[i/8][i%8]='b';
        }
        if (((board.blackRooks>>i)&1)==1) {
            chessBoard[i/8][i%8]='r';
        }
        if (((board.blackQueens>>i)&1)==1) {
            chessBoard[i/8][i%8]='q';
        }
        if (((board.blackKing>>i)&1)==1) {
            chessBoard[i/8][i%8]='k';
        }
    }
    for (int i=0;i<64;i++) {
        printf("%c", chessBoard[i/8][i%8]);
        if(i%8 == 7) {
            printf("\n");
        }
    }
}

void drawArrayNumbered(struct Board board) {
    char chessBoard[8][8];
    for (int i=0;i<64;i++) {
        chessBoard[i/8][i%8]=' ';
    }
    for (int i=0;i<64;i++) {
        if (((board.whitePawns>>i)&1)==1) {
            chessBoard[i/8][i%8]='P';
        }
        if (((board.whiteKnights>>i)&1)==1) {
            chessBoard[i/8][i%8]='N';}
        if (((board.whiteBishops>>i)&1)==1) {chessBoard[i/8][i%8]='B';
        }
        if (((board.whiteRooks>>i)&1)==1) {
            chessBoard[i/8][i%8]='R';
        }
        if (((board.whiteQueens>>i)&1)==1) {
            chessBoard[i/8][i%8]='Q';}
        if (((board.whiteKing>>i)&1)==1) {chessBoard[i/8][i%8]='K';
        }
        if (((board.blackPawns>>i)&1)==1) {
            chessBoard[i/8][i%8]='p';}
        if (((board.blackKnights>>i)&1)==1) {chessBoard[i/8][i%8]='n';
        }
        if (((board.blackBishops>>i)&1)==1) {
            chessBoard[i/8][i%8]='b';
        }
        if (((board.blackRooks>>i)&1)==1) {
            chessBoard[i/8][i%8]='r';
        }
        if (((board.blackQueens>>i)&1)==1) {
            chessBoard[i/8][i%8]='q';
        }
        if (((board.blackKing>>i)&1)==1) {
            chessBoard[i/8][i%8]='k';
        }
    }
    printf("    a   b   c   d   e   f   g   h\n");
    for (int i=0;i<64;i++) {
        
        
    	if(i%8 == 0) {
            printf("  +---+---+---+---+---+---+---+---+\n");
            if(i/8 % 2 == 0) {
                printf("  |\033[1;35m~~~\033[0;37m|   |\033[1;35m~~~\033[0;37m|   |\033[1;35m~~~\033[0;37m|   |\033[1;35m~~~\033[0;37m|   |");
            }
            else {
                printf("  |   |\033[1;35m~~~\033[0;37m|   |\033[1;35m~~~\033[0;37m|   |\033[1;35m~~~\033[0;37m|   |\033[1;35m~~~\033[0;37m|");
            }
            printf(" %d\n", 8 - i/8);
            printf("%d ", 8 - i/8);
        }
        if((i - i/8) % 2 == 0) {
            printf("|\033[1;35m~\033[0;37m");
        }
        else {
            printf("| ");
        }
        if(oldChessBoard[i/8][i%8] != chessBoard[i/8][i%8] && chessBoard[i/8][i%8] == ' ') {
            printf("\033[1;31m");
            if((i - i/8) % 2 == 0) {
                printf("X\033[1;35m~\033[0;37m");
            }
            else{
                printf("X ");
            }
        }
        else {
            if(chessBoard[i/8][i%8] > 'a') {
                printf("\033[1;36m");
            }
            else {
                printf("\033[1;33m");
            }
            if((i - i/8) % 2 == 0) {
                if(chessBoard[i/8][i%8] != ' ') {
                    printf("%c", chessBoard[i/8][i%8]);
                }
                else {
                    printf("\033[1;35m~\033[0;37m");
                }
                printf("\033[1;35m~\033[0;37m");
            }
            else {
                printf("%c ", chessBoard[i/8][i%8]);
            }
            printf("\x1b\x5b\x30\x6d");
        }
        printf("\x1b\x5b\x30\x6d");
        if(i%8 == 7) {
            printf("|\n");
            
        }
    }
    printf("  +---+---+---+---+---+---+---+---+\n");
    printf("    a   b   c   d   e   f   g   h\n");
    
    for (int i=0;i<64;i++) {
        oldChessBoard[i/8][i%8] = chessBoard[i/8][i%8];
    }
}
