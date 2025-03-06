#include "moves.h"

#include "board.h"

#define MAX_RESPONSES 20

struct OpeningNode {
    struct Move move;
    int weight;
    int responseWeightSum;
    int responseCount;

    struct OpeningNode * responses[MAX_RESPONSES];
};

void generateOpeningBook(void);
struct Move pgnToMove(struct Board * board, char * s, bool WhiteToMove);
bool equalMoves(struct Move move1, struct Move move2);
