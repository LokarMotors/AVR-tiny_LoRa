#include "CAY_DIY.h"

CAYDIY::CAYDIY(unsigned char size)
{
    Buffer = (unsigned char *)malloc(size * 4);
}

void CAYDIY::Add(unsigned char channel, unsigned char type, int data)
{
    *(Buffer + Index) = channel;
    *(Buffer + Index + 1) = type;
    *(Buffer + Index + 2) = data >> 8;
    *(Buffer + Index + 3) = data;
    Index += 4;
}