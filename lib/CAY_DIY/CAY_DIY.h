#ifndef CAY_DIY_h
#define CAY_DIY_h

#include <stdlib.h> 

class CAYDIY
{
    public:
    CAYDIY(unsigned char size);
    void Add(unsigned char channel, unsigned char type, int data);
    unsigned char Index = 0;
    unsigned char *Buffer;
   
};

#endif