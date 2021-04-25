#include "str_util.h"

#include <string.h>


char* str_find_last_of(char* str, char* identifier)
{
    char* last = NULL;
    size_t n = strlen(identifier);
    while (*str)
    {
        if (!memcmp(str++, identifier, n))
        {
            last = str - 1;
        }
    }
    return last;
}

char* str_trunc(char* str, int pos)
{
    size_t len = strlen(str);

    if (len > abs(pos)) {
        if (pos > 0)
            str = str + pos;
        else
            str[len + pos] = 0;
    }

    return str;
}