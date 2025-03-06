#include "moves.h"
#include "boardbuilder.h"
#include "perft.h"
#include <stdio.h>

/*
moveToAlgebra(String move)
{
    String moveString="";
    moveString+=""+(char)(move.charAt(1)+49);
    moveString+=""+('8'-move.charAt(0));
    if(Character.isDigit(move.charAt(3))) {
        moveString+=""+(char)(move.charAt(3)+49);
    }
    else moveString+=move.charAt(3);
    if(Character.isDigit(move.charAt(2))) {
        moveString+=""+('8'-move.charAt(2));
    }
    else moveString+=move.charAt(2);
    return moveString;
}
*/

int perftTotalMoveCounter=0;
int perftMoveCounter=0;
int perftMaxDepth=6;
void perftRoot(struct Board * board, bool WhiteToMove, int depth)
{
    struct MoveList moves;
    struct Board nextBoard;
    moves.size = 0;
    if (WhiteToMove) {
        possibleMovesWhite(&moves, board);
    } else {
        possibleMovesBlack(&moves, board);
    }
    for (int i=0;i<moves.size;i++) {
    	nextBoard.whitePawns = move(board->whitePawns, moves.moves[i], WHITE_PAWN);
        nextBoard.whiteKnights = move(board->whiteKnights, moves.moves[i], WHITE_KNIGHT);
        nextBoard.whiteBishops = move(board->whiteBishops, moves.moves[i], WHITE_BISHOP);
        nextBoard.whiteRooks = move(board->whiteRooks, moves.moves[i], WHITE_ROOK);
        nextBoard.whiteQueens = move(board->whiteQueens, moves.moves[i], WHITE_QUEEN);
        nextBoard.whiteKing = move(board->whiteKing, moves.moves[i], WHITE_KING);
        
        nextBoard.blackPawns = move(board->blackPawns, moves.moves[i], BLACK_PAWN);
        nextBoard.blackKnights = move(board->blackKnights, moves.moves[i], BLACK_KNIGHT);
        nextBoard.blackBishops = move(board->blackBishops, moves.moves[i], BLACK_BISHOP);
        nextBoard.blackRooks = move(board->blackRooks, moves.moves[i], BLACK_ROOK);
        nextBoard.blackQueens = move(board->blackQueens, moves.moves[i], BLACK_QUEEN);
        nextBoard.blackKing = move(board->blackKing, moves.moves[i], BLACK_KING);
        
        nextBoard.enPassant = move(board->whitePawns|board->blackPawns, moves.moves[i], BLACK_KING);
        
        nextBoard.whiteRooks = makeMoveCastle(nextBoard.whiteRooks, board->whiteKing, moves.moves[i], WHITE_ROOK);
        nextBoard.blackRooks = makeMoveCastle(nextBoard.blackRooks, board->blackKing, moves.moves[i], BLACK_ROOK);
        
        nextBoard.whiteKingKingsideCastle = board->whiteKingKingsideCastle;
        nextBoard.whiteKingQueensideCastle = board->whiteKingQueensideCastle;
        nextBoard.blackKingKingsideCastle = board->blackKingKingsideCastle;
        nextBoard.blackKingQueensideCastle = board->blackKingQueensideCastle;
        
        drawArray(nextBoard);
        
        if (moves.moves[i].endFile <= VAL_FILE_H) {//'regular' move
            int start = moves.moves[i].startRank * 8 + moves.moves[i].startFile;
            
            if (((1L<<start)&board->whiteKing)!=0) {nextBoard.whiteKingKingsideCastle=false; nextBoard.whiteKingQueensideCastle=false;}
            else if (((1L<<start)&board->blackKing)!=0) {nextBoard.blackKingKingsideCastle=false; nextBoard.blackKingQueensideCastle=false;}
            else if (((1L<<start)&board->whiteRooks&(1L<<CASTLE_ROOKS[0]))!=0) {nextBoard.whiteKingKingsideCastle=false;}
            else if (((1L<<start)&board->whiteRooks&(1L<<CASTLE_ROOKS[1]))!=0) {nextBoard.whiteKingQueensideCastle=false;}
            else if (((1L<<start)&board->blackRooks&(1L<<CASTLE_ROOKS[2]))!=0) {nextBoard.blackKingKingsideCastle=false;}
            else if (((1L<<start)&board->blackRooks&1L)!=0) {nextBoard.blackKingQueensideCastle=false;}
        }
        if (((nextBoard.whiteKing&blackAttacks(nextBoard.whitePawns,nextBoard.whiteKnights,nextBoard.whiteBishops,nextBoard.whiteRooks,nextBoard.whiteQueens,nextBoard.whiteKing,nextBoard.blackPawns,nextBoard.blackKnights,nextBoard.blackBishops,nextBoard.blackRooks,nextBoard.blackQueens,nextBoard.blackKing))==0 && WhiteToMove) ||
            ((nextBoard.blackKing&whiteAttacks(nextBoard.whitePawns,nextBoard.whiteKnights,nextBoard.whiteBishops,nextBoard.whiteRooks,nextBoard.whiteQueens,nextBoard.whiteKing,nextBoard.blackPawns,nextBoard.blackKnights,nextBoard.blackBishops,nextBoard.blackRooks,nextBoard.blackQueens,nextBoard.blackKing))==0 && !WhiteToMove)) {
            perft(&nextBoard,!WhiteToMove,depth+1);
            //System.out.println(moveToAlgebra(moves.substring(i,i+4))+" "+perftMoveCounter);
            printf("\n%d\n", perftMoveCounter);
            perftTotalMoveCounter+=perftMoveCounter;
            perftMoveCounter=0;
        }
    }
}
void perft(struct Board * board, bool WhiteToMove, int depth)
{
	if (depth<perftMaxDepth) {
    		struct MoveList moves;
    		struct Board nextBoard;
    		moves.size = 0;
   		if (WhiteToMove) {
			possibleMovesWhite(&moves, board);
    		} else {
        		possibleMovesBlack(&moves, board);
    		}
    		for (int i=0;i<moves.size;i++) {
        		nextBoard.whitePawns = move(board->whitePawns, moves.moves[i], WHITE_PAWN);
        		nextBoard.whiteKnights = move(board->whiteKnights, moves.moves[i], WHITE_KNIGHT);
        		nextBoard.whiteBishops = move(board->whiteBishops, moves.moves[i], WHITE_BISHOP);
        		nextBoard.whiteRooks = move(board->whiteRooks, moves.moves[i], WHITE_ROOK);
        		nextBoard.whiteQueens = move(board->whiteQueens, moves.moves[i], WHITE_QUEEN);
        		nextBoard.whiteKing = move(board->whiteKing, moves.moves[i], WHITE_KING);
        
       		nextBoard.blackPawns = move(board->blackPawns, moves.moves[i], BLACK_PAWN);
        		nextBoard.blackKnights = move(board->blackKnights, moves.moves[i], BLACK_KNIGHT);
        		nextBoard.blackBishops = move(board->blackBishops, moves.moves[i], BLACK_BISHOP);
        		nextBoard.blackRooks = move(board->blackRooks, moves.moves[i], BLACK_ROOK);
        		nextBoard.blackQueens = move(board->blackQueens, moves.moves[i], BLACK_QUEEN);
        		nextBoard.blackKing = move(board->blackKing, moves.moves[i], BLACK_KING);
        
        		nextBoard.enPassant = move(board->whitePawns|board->blackPawns, moves.moves[i], BLACK_KING);
        
        		nextBoard.whiteRooks = makeMoveCastle(nextBoard.whiteRooks, board->whiteKing, moves.moves[i], WHITE_ROOK);
        		nextBoard.blackRooks = makeMoveCastle(nextBoard.blackRooks, board->blackKing, moves.moves[i], BLACK_ROOK);
        

        		nextBoard.whiteKingKingsideCastle = board->whiteKingKingsideCastle;
        		nextBoard.whiteKingQueensideCastle = board->whiteKingQueensideCastle;
        		nextBoard.blackKingKingsideCastle = board->blackKingKingsideCastle;
        		nextBoard.blackKingQueensideCastle = board->blackKingQueensideCastle;
        
        		if (moves.moves[i].endFile <= VAL_FILE_H) {//'regular' move
            			int start = moves.moves[i].startRank * 8 + moves.moves[i].startFile;
            

            			if (((1L<<start)&board->whiteKing)!=0) {nextBoard.whiteKingKingsideCastle=false; nextBoard.whiteKingQueensideCastle=false;}
            			else if (((1L<<start)&board->blackKing)!=0) {nextBoard.blackKingKingsideCastle=false; nextBoard.blackKingQueensideCastle=false;}
            			else if (((1L<<start)&board->whiteRooks&(1L<<CASTLE_ROOKS[0]))!=0) {nextBoard.whiteKingKingsideCastle=false;}

            			else if (((1L<<start)&board->whiteRooks&(1L<<CASTLE_ROOKS[1]))!=0) {nextBoard.whiteKingQueensideCastle=false;}
            			else if (((1L<<start)&board->blackRooks&(1L<<CASTLE_ROOKS[2]))!=0) {nextBoard.blackKingKingsideCastle=false;}
            			else if (((1L<<start)&board->blackRooks&1L)!=0) {nextBoard.blackKingQueensideCastle=false;}
       		}
        		if 	(((nextBoard.whiteKing&blackAttacks(nextBoard.whitePawns,nextBoard.whiteKnights,nextBoard.whiteBishops,nextBoard.whiteRooks,nextBoard.whiteQueens,nextBoard.whiteKing,nextBoard.blackPawns,nextBoard.blackKnights,nextBoard.blackBishops,nextBoard.blackRooks,nextBoard.blackQueens,nextBoard.blackKing))==0 && WhiteToMove) ||
         ((nextBoard.blackKing&whiteAttacks(nextBoard.whitePawns,nextBoard.whiteKnights,nextBoard.whiteBishops,nextBoard.whiteRooks,nextBoard.whiteQueens,nextBoard.whiteKing,nextBoard.blackPawns,nextBoard.blackKnights,nextBoard.blackBishops,nextBoard.blackRooks,nextBoard.blackQueens,nextBoard.blackKing))==0 && !WhiteToMove)) {
         			if(depth+1 == perftMaxDepth) perftMoveCounter++;
            			perft(&nextBoard,!WhiteToMove,depth+1);
            		}
        }
    }
}
