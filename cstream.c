#include <stdlib.h>
#include <ctype.h>

#include "cstream.h"

// open up a new character stream pointing to a certain file
cstream* cstream_open(const char* fp)
{
    cstream* stream = malloc(sizeof(cstream));
    if(stream == NULL) return NULL;
    stream->line = 0;
    stream->col = 0;
    FILE* f = fopen(fp, "r");
    if(f == NULL) cstream_croak(stream, "couldn't open file");
    stream->f = f;
}

// close and free a character stream
void cstream_close(cstream* stream)
{
    if(stream->f != NULL) fclose(stream->f);
    if(stream != NULL) free(stream);
}

// throw an error
void cstream_croak(cstream* stream, const char* msg)
{
    fprintf(stderr, "Error: %s (l-%i:c-%i)\n", msg, stream->line + 1, stream->col);
    cstream_close(stream);
    exit(EXIT_FAILURE);
}

// peek the next character in the stream
int cstream_peek(cstream* stream)
{
    int c = getc(stream->f);
    ungetc(c, stream->f);
    return c;
}

// pop the next character in the stream
int cstream_pop(cstream* stream)
{
    int c = getc(stream->f);
    if(c == '\n') stream->line++;
    else stream->col++;
    return c;
}

// skip until end of whitespace
void cstream_skipspace(cstream* stream)
{
    while(cstream_peek(stream) != EOF && isspace(cstream_peek(stream))) cstream_pop(stream);
}

void cstream_push(cstream* stream, char c)
{
    ungetc(c, stream->f);
    if(c == '\n') stream->line--;
    else stream->col--;
}
