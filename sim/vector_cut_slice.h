#include <cmath>
#include <iostream>

using namespace std;

int *slice(uint64_t source){
    static int  bit[64];
    for(int i = 0; i < 64; i++){
        bit[i] = source % 2;
        source = source >> 1;
    }
    return bit;
};
