#include "common.h"

char *copy(char *dst, char *src, size_t size)
{
    size_t i;
    for(i = 0; i < size; i++)
    {
        dst[i] = src[i];
        if(src[i] == 0) break;
    }
    return dst;
}

char *concat(char *dst, char *src, size_t size)
{
    size_t i = 0, j = 0;
    for(; i < size; i++)
    {
        if(dst[i] == 0) break;
    }
    for(; i < size; i++)
    {
        dst[i] = src[j++];
        if(src[j - 1] == 0) break;
    }
    dst[size-1] = 0;
    return dst;
}
