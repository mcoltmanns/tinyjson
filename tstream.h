#include "cstream.h"

#define ispunc(c) (c == '{' || c == '}' || c == '[' || c == ']' || c == ':' || c == ',')
#define isnumeric(c) (isdigit(c) || c == '-' || c == '+' || c == 'e' || c == 'E' || c == '.')

typedef struct { // tokens hold their type their value
    int type;
    union {
        char punc; // {}, [], (,), :
        char* str; // any null terminated string
        int boolean; // 0 false, !0 true
        double num; // any double
    };
} token;

enum TOKEN_TYPES {
    PUNC, // {}, [], (,), :
    STR,
    NUM,
    BOOL // true or false
};

// destroy a token
void token_destroy(token* t);

// get a string representation of a token
char* token_tostr(token* t);

// read the next token off the stream and advance
token* token_next(cstream* stream);