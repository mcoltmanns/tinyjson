#include "parser.h"
#include <stdlib.h>
#include <string.h>

void main()
{
    cstream* stream = cstream_open("10.11.99.1.json");
    token* t = token_next(stream);
    while(t)
    {
        printf("%s\n", token_tostr(t));
        t = token_next(stream);
        token_destroy(t);
    }
    cstream_close(stream);
}