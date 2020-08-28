#include<stdio.h>
unsigned rand_num_gen()
{
    unsigned short lfsr = 0xACE1u;
    unsigned bit;

    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
    return lfsr = (lfsr >> 1) | (bit << 15);
}

int main(){
    unsigned n = rand_num_gen();
    printf('%i', n);
    return 0;
}