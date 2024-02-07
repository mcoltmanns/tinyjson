#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tstream.h"

// allocate a punctuation token
token* token_createpunc(const char value)
{
    token* t = malloc(sizeof(token));
    if(t == NULL) return NULL;
    t->type = PUNC;
    t->punc = value;
    return t;
}

// allocate a string token
token* token_createstr(char* value)
{
    token* t = malloc(sizeof(token));
    if(t == NULL) return NULL;
    t->type = STR;
    t->str = value;
    //t->str = malloc(strlen(value) + 1);
    //if(t->str == NULL) return NULL;
    //strcpy(t->str, value);
    return t;
}

// allocate a number token
token* token_createnum(const double value)
{
    token* t = malloc(sizeof(token));
    if(t == NULL) return NULL;
    t->type = NUM;
    t->num = value;
    return t;
}

// allocate a boolean token
token* token_createbool(const int value)
{
    token* t = malloc(sizeof(token));
    if(t == NULL) return NULL;
    t->type = BOOL;
    t->boolean = value;
    return t;
}

// destroy a token
void token_destroy(token* t)
{
    if(t == NULL) return; // don't try to free null tokens
    if(t->type == STR) free(t->str); // if it was a string, free the string
    free(t);
}

// get a string representation of a token
char* token_tostr(token* t)
{
    if(t == NULL) return "";

    switch (t->type)
    {
        case PUNC:
            static char str[2];
            sprintf(str, "%c", t->punc);
            return str;
        case STR:
            return t->str;
        case NUM:
            static char s[32];
            sprintf(s, "%f", t->num);
            return s;
        case BOOL:
            if(t->boolean) return "true";
            else return "false";
    }
}

// read the next string literal from the stream (stream should be positioned at the opening quote)
char* read_str(cstream* stream)
{
    if(cstream_peek(stream) == EOF)
            cstream_croak(stream, "incomplete string (eof)");
    int length = 0;
    int bufsize = 1;
    char* s = calloc(bufsize, 1);
    if(s == NULL)
        cstream_croak(stream, "out of memory");

    if(cstream_pop(stream) != '"')
        cstream_croak(stream, "expected a string");

    while(cstream_peek(stream) != '"') // while not at end of string
    {
        if(cstream_peek(stream) == EOF)
            cstream_croak(stream, "incomplete string (eof)");
        length++;
        s[length - 1] = cstream_pop(stream);
        if(length + 1 > bufsize) // grow string if needed
        {
            bufsize += 1;
            s = realloc(s, bufsize);
            if(s == NULL)
                cstream_croak(stream, "out of memory");
        }
    }
    s[length] = 0;
    if(cstream_peek(stream) == EOF)
            cstream_croak(stream, "incomplete string (eof)");
    cstream_pop(stream); // skip closing quotes
    return s;
}

// read the next boolean literal from the stream (stream should be positioned at 't' or 'f')
int read_bool(cstream* stream)
{
    if(cstream_peek(stream) == EOF)
            cstream_croak(stream, "incomplete boolean (eof)");
    char actual[6];
    char* expected;
    int size;
    if(cstream_peek(stream) == 't') // expecting true
    {
        expected = "true";
        size = sizeof("true");
    }
    else if(cstream_peek(stream) == 'f') // expecting false
    {
        expected = "false";
        size = sizeof("false");
    }
    else // not a boolean
        cstream_croak(stream, "expected a boolean");
        
    for(int i = 0; i < size - 1; i++) // read in actual value
    {
        if(cstream_peek(stream) == EOF)
            cstream_croak(stream, "incomplete boolean (eof)");
        actual[i] = cstream_pop(stream);
    }
    actual[size - 1] = 0; // terminate actual value string
    if(strcmp(actual, expected)) // if expected value doesn't match actual value, it isn't a valid boolean
        cstream_croak(stream, "expected a boolean");
    return !strcmp(expected, "true"); // if expected value is true return true, otherwise return false
}

// read the next number literal from the stream (stream should be positioned at the first digit of the number or its sign)
double read_num(cstream* stream)
{
    char last = cstream_peek(stream);
    char c = cstream_pop(stream);
    if((c == '-' || c == '+') && !isdigit(cstream_peek(stream)))
        cstream_croak(stream, "lone signs not allowed");
    char* s = calloc(1, 1);
    int length = 0;
    int hasdecimal = 0;
    int hasexp = 0;
    while(isnumeric(c))
    {
        if(((c == '-' || c == '+') && last != 'E' && last != 'e') && length != 0 || (c == '.' || c == 'e' || c == 'E') && !isnumeric(cstream_peek(stream))) // signs not allowed outside of first place unless preceeded by an exponent, and decimals and exponents not allowed in last place
            cstream_croak(stream, "malformed number");
        if(c == '.')
        {
            if(hasexp || hasdecimal) // no double decimals, and no decimals after exponents
                cstream_croak(stream, "malformed number");
            hasdecimal = 1;
        }
        if(c == 'e' || c == 'E')
        {
            if(hasexp) // no double exponents
                cstream_croak(stream, "malformed number");
            hasexp = 1;
        }
        s[length] = c;
        length++;
        s = realloc(s, length + 1);
        last = c;
        c = cstream_pop(stream);
    }
    s[length] = 0;
    double out = strtod(s, NULL);
    return out;
}

// read the next token from the stream
token* token_next(cstream* stream)
{
    cstream_skipspace(stream); // skip whitespace
    char c = cstream_peek(stream);
    if(c == EOF) return NULL;
    if(ispunc(c)) // if c is punctuation ({} [] : ,)
    {
        char c = cstream_pop(stream);
        return token_createpunc(c); // create a punctuation token
    }
    if(c == '"') // if c is a string opener
        return token_createstr(read_str(stream)); // create a string token, and read in the next string on the stream
    if(c == 't' || c == 'f') // if c is the start of a boolean
        return token_createbool(read_bool(stream)); // create a boolean token, and read in the next boolean on the stream
    if(c == '+' || c == '-' || isdigit(c))
        return token_createnum(read_num(stream)); // create a number token, and read in the next number on the stream
    char err[] = {'u','n','k','n','o','w','n',' ','t','o','k','e','n',' ','\"',c,'\"',0};
    cstream_croak(stream, err);
    return NULL;
}
