/*
just parse from an array of characters
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define SUCCESS 1
#define FAILURE 0

enum {
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_STRING,
    JSON_NUMBER,
    JSON_BOOL,
    JSON_NULL
};

typedef struct jvalue jvalue;
typedef struct jmember jmember;

struct jvalue {
    int type;
    union {
        jmember* members; // objects are linked lists of members (last member points to null)
        jvalue** elements; // arrays are arrays of pointers to jvalues (terminate with null ptr)
        char* string;
        double number;
        int boolean;
    };
};

struct jmember {
    char* string; // members contain a string and a pointer to an element, and a pointer to the next member in the object
    jvalue* element;
    jmember* next;
};

// what is this const char** cursor?
// a mutable pointer->pointer-> an immutable character

// advance the cursor until it isn't on a space anymore
static void skip_space(const char** cursor)
{
    if(**cursor == '\0') return; // stop on eof
    while(isspace(**cursor)) (*cursor)++; // as long as the cursor is on a space, increment the cursor's position
}

// advance the cursor to the next occurrence of a character
static void advance_to(char target, const char** cursor)
{
    if(**cursor == '\0') return;
    while(**cursor != target) (*cursor)++;
}

static void json_free_value(jvalue* v);
static void json_free_member(jmember* m);

static void json_free_value(jvalue* v) // TODO: review edge cases here. what about freeing partially/malformed values?
{
    switch(v->type) // following 3 cases are dynamically allocated
    {
        jmember* next;
        case JSON_OBJECT:
            while(v->members != NULL) // go until null pointer
            {
                next = v->members;
                v->members = v->members->next;
                json_free_member(next);
            }
            free(v->members); // free the head pointer
            break;

        case JSON_ARRAY: // same concept as for objects
            for(int i = 0; v->elements[i] != NULL; i++)
                json_free_value(v->elements[i]);
            free(v->elements);
            break;

        case JSON_STRING:
            free(v->string);
            break;
    }

    free(v); // free this struct
}

static void json_free_member(jmember* m)
{
    free(m->string);
    json_free_value(m->element);
    free(m);
}

// check if the text at cursor is a certain literal, and advance cursor to the character after that literal
static int json_is_literal(const char** cursor, const char* literal)
{
    while(*literal != '\0' && !isspace(**cursor))
    {
        if(*literal != **cursor) return FAILURE;
        literal ++;
        (*cursor)++;
    }
    return SUCCESS;
}

static int json_parse_member(const char** cursor, jmember* member);
static int json_parse_value(const char** cursor, jvalue* empty);
static void json_print_value(const jvalue* v);

// assume cursor starts immediately after this object's opening bracket
static int json_parse_member(const char** cursor, jmember* member)
{
    skip_space(cursor); // chop whitespace
    // read in the key
    if(**cursor != '"') return FAILURE; // look for the opening quote
    (*cursor)++;
    const char* start = *cursor; // parse in the object key
    advance_to('"', cursor); // find the end of the string
    long int length = *cursor - start; // figure length
    member->string = calloc(length + 1, 1); // allocate the key
    if(member->string == NULL) return FAILURE;
    memcpy(member->string, start, length); // copy in the key
    (*cursor)++; // advance the cursor past the closing quote
    skip_space(cursor);
    if(**cursor != ':') return FAILURE; // look for the colon
    (*cursor)++; // advance the cursor past the colon
    // read in the value
    member->element = malloc(sizeof(jvalue));
    if(member->element == NULL) return FAILURE;
    return json_parse_value(cursor, member->element);
}

static int json_parse_value(const char** cursor, jvalue* empty)
{
    skip_space(cursor); // chop whitespace
    if(**cursor == '\0') return FAILURE; // can't parse on eof
    switch(**cursor)
    {
        case '{': // parse an object TODO: review edge cases here, and fix that ugly loop
            empty->type = JSON_OBJECT;
            empty->members = NULL; // initialize an empty object (point head to null)
            (*cursor)++; // go to next character
            skip_space(cursor); // skip any space before first member
            if(**cursor == '}') return SUCCESS; // if the object closes immediately stop
            jmember* tail = empty->members; // points to NULL (because at first there is no tail)
            while(1) // go until object close
            {
                jmember* newMember = malloc(sizeof(jmember));
                if(newMember == NULL) return FAILURE;
                if(json_parse_member(cursor, newMember)) // try and parse in the next member
                {
                    newMember->next = NULL; // new element is going at the tail
                    if(tail == NULL) empty->members = newMember; // if there wasn't a tail, point the head to the new element
                    else tail->next = newMember; // if there was a tail, point it to the new element
                    tail = newMember; // set the tail to the new element
                }
                else return FAILURE;
                skip_space(cursor); // skip until the next thing
                if(**cursor == '}') break; // stop when encountering a closing bracket
                else if(**cursor != ',') return FAILURE; // fail when not finding a comma
                (*cursor)++; // increment the cursor
                skip_space(cursor);
            }
            (*cursor)++; // continue to the next thing
            return SUCCESS;

        case '[': // parse an array
            empty->type = JSON_ARRAY;
            int size = 4;
            empty->elements = calloc(size, sizeof(jvalue*)); // allocate space for 4 pointers (all set to null)
            if(empty->elements == NULL) return FAILURE;
            (*cursor)++;
            skip_space(cursor);
            if(**cursor == ']') return SUCCESS; // if object closes immediately, we're done
            int i = 0;
            while(1) // TODO: also fix this ugly loop
            {
                jvalue* newValue = malloc(sizeof(jvalue));
                if(newValue == NULL) return FAILURE;
                if(json_parse_value(cursor, newValue)) // try to parse a value
                {
                    if(i + 1 == size) // are we about to overflow?
                    {
                        size *= 2; // double the size
                        jvalue** moreSpace = realloc(empty->elements, size);
                        if(moreSpace == NULL) return FAILURE; // external caller should handle deallocation anyways
                        empty->elements = moreSpace; // have to twostep here so that external deallocation can find the old array in case of failure
                    }
                    empty->elements[i] = newValue;
                    i++;
                }
                else return FAILURE;
                skip_space(cursor); // skip until the next thing
                if(**cursor == ']') break; // stop when encountering a closing bracket
                else if(**cursor != ',') return FAILURE; // fail when not finding a comma
                (*cursor)++; // increment the cursor
                skip_space(cursor);
            }
            empty->elements[i] = NULL; // terminate the array
            jvalue** trimmed = realloc(empty->elements, i); // free any unused space
            if(trimmed == NULL) return FAILURE;
            empty->elements = trimmed; // again twostep so caller can handle deallocation on failure
            (*cursor)++; // continue to the next thing
            return SUCCESS;

        case '"': // parse a string
            (*cursor)++;
            const char* start = *cursor;
            advance_to('"', cursor); // find the end of the string
            empty->string = calloc(*cursor - start + 1, 1); // allocate the string
            if(empty->string == NULL) return FAILURE;
            memcpy(empty->string, start, *cursor - start); // copy the string
            empty->type = JSON_STRING; // set object type
            (*cursor)++; // scoot the cursor past the end of the string (skip closing quotes)
            return SUCCESS; // done!

        case 't': // parse a true literal
            if(json_is_literal(cursor, "true")) // takes care of cursor movement
            {
                empty->type = JSON_BOOL;
                empty->boolean = 1;
                return SUCCESS;
            }
            return FAILURE;

        case 'f': // parse a false literal
            if(json_is_literal(cursor, "false")) // takes care of cursor movement
            {
                empty->type = JSON_BOOL;
                empty->boolean = 0;
                return SUCCESS;
            }
            return FAILURE;
        
        case 'n': // parse a null literal
            if(json_is_literal(cursor, "null")) // takes care of cursor movement
            {
                empty->type = JSON_NULL;
                return SUCCESS;
            }
            return FAILURE;

        default: // parse a number literal
            empty->type = JSON_NUMBER;
            char before = **cursor; // FIXME: funky pointers!
            empty->number = strtod(*cursor, cursor);
            return (empty->number == 0 && **cursor == before) ? FAILURE : SUCCESS; // TODO: make sure this check actually catches everything!
    }
}

static void json_print_value(const jvalue* v) // TODO: would be nice if this did fancy indentation
{
    if(v == NULL) return;
    switch(v->type)
    {
        case JSON_OBJECT:
            printf("{\n");
            jmember* now = v->members;
            while(now != NULL)
            {
                printf("\"%s\" : ", now->string);
                json_print_value(now->element);
                if(now->next != NULL) printf(",\n");
                now = now->next;
            }
            printf("\n}");
            break;
        
        case JSON_ARRAY:
            printf("[\n");
            for(int i = 0; v->elements[i] != NULL; i++)
            {
                json_print_value(v->elements[i]);
                if(v->elements[i + 1] != NULL) printf(",\n");
            }
            printf("\n]");
            break;

        case JSON_STRING:
            printf("\"%s\"", v->string);
            break;

        case JSON_NUMBER:
            printf("%f", v->number);
            break;

        case JSON_BOOL:
            printf("%s", v->boolean ? "true" : "false");
            break;

        case JSON_NULL:
            printf("null");
            break;
    }
}