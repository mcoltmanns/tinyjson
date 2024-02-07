#include <stdio.h>

typedef struct {
    int line, col;
    FILE* f;
} cstream;

// open up a new character stream pointing to a certain file
cstream* cstream_open(const char* fp);

// close and free a character stream
void cstream_close(cstream* stream);

// peek the next character in the stream
int cstream_peek(cstream* stream);

// pop the next character in the stream
int cstream_pop(cstream* stream);

// push a character to the stream
void cstream_push(cstream* stream, char c);

// throw an error and crash
void cstream_croak(cstream* stream, const char* msg);

// skip until end of whitespace
void cstream_skipspace(cstream* stream);
