#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define Gp 0x5d99b5dd391ea081
#define Gq 0x1fd50acabb22ec59
#define NUM_MAX 0xFFFFFFFF
typedef uint32_t numberType;

numberType ps(numberType gseed);
bool distigunister(numberType y);

int main() {
    
    long long total = 0;
    long long count = 0;
    for(numberType i = 0; i < NUM_MAX; i++) {
        
        bool R = distigunister(i);
        
        if(R) {
            count++;
        }
        total++;
    }
    printf("%lld %lld\n", total, count);
    
    total = 0;
    count = 0;
    for(numberType i = 0; i < NUM_MAX; i++) {
        numberType psOutput = ps(i);
        
        bool R = distigunister(psOutput);
        
        
        if(R) {
            count++;
        }
        total++;
    }
    printf("%lld %lld\n", total, count);
}

numberType ps(numberType gseed) {
    if(__builtin_parity(gseed)) {
        return gseed ^ Gp;
    }
    else{
        return gseed ^ Gq;
    }
}

bool distigunister(numberType y) {
    numberType p = y ^ Gp;
    numberType q = y ^ Gq;
    bool j = __builtin_parity(p);
    bool h = !__builtin_parity(q);
    return (h || j);
}
