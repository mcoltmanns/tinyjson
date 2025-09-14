#include "tinyjson.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// what is this const char** cursor?
// a mutable pointer->pointer-> a mutable character

// advance the cursor until it isn't on a space anymore
static void skip_space(char** cursor)
{
    if(**cursor == '\0') return; // stop on eof
    while(isspace(**cursor)) (*cursor)++; // as long as the cursor is on a space, increment the cursor's position
}

// advance the cursor to the next occurrence of a character
static void advance_to(char target, char** cursor)
{
    if(**cursor == '\0') return;
    while(**cursor != target) (*cursor)++;
}

// free the memory associated with a jmember
// frees the string, then the value with json_free_value(), then the member itself
static void json_free_member(jmember* m)
{
    free(m->string);
    json_free_value(m->element);
    free(m);
}

// check if the text at cursor is a certain literal, and advance cursor to the character after that literal
static int json_is_literal(char** cursor, const char* literal)
{
    while(*literal != '\0' && !isspace(**cursor))
    {
        if(*literal != **cursor) return JSON_FAILURE;
        literal ++;
        (*cursor)++;
    }
    return JSON_SUCCESS;
}

void json_free_value(jvalue* v) // TODO: review edge cases here. what about freeing partially/malformed values?
{
    if(v == NULL) return;
    switch(v->type) // following 3 cases are dynamically allocated
    {
        case JSON_OBJECT:
            while(v->members != NULL) // go until null pointer
            {
                jmember *next = v->members;
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

        case JSON_STRING: // for strings just free the string
            free(v->string);
            break;

        default: // in the default case, do nothing (for statically alloc'd structs)
            break;
    }

    free(v); // free the static parts
}

// assume cursor starts immediately after this object's opening bracket
static int json_parse_member(char** cursor, jmember* member)
{
    skip_space(cursor); // chop whitespace
    // read in the key
    if(**cursor != '"') return JSON_FAILURE; // look for the opening quote
    (*cursor)++;
    const char* start = *cursor; // parse in the object key
    advance_to('"', cursor); // find the end of the string
    long int length = *cursor - start; // figure length
    member->string = calloc(length + 1, 1); // allocate the key
    if(member->string == NULL) return JSON_FAILURE;
    memcpy(member->string, start, length); // copy in the key
    (*cursor)++; // advance the cursor past the closing quote
    skip_space(cursor);
    if(**cursor != ':') return JSON_FAILURE; // look for the colon
    (*cursor)++; // advance the cursor past the colon
    // read in the value
    member->element = malloc(sizeof(jvalue));
    if(member->element == NULL) return JSON_FAILURE;
    return json_parse_value(cursor, member->element);
}

void json_print_value(jvalue* v)
{
    char* str = jval_to_str(v);
    printf("%s\n", str);
    free(str);
}

int json_parse_value(char** cursor, jvalue* empty)
{
    skip_space(cursor); // chop whitespace
    if(**cursor == '\0') return JSON_FAILURE; // can't parse on eof
    switch(**cursor)
    {
        case '{': // parse an object TODO: review edge cases here, and fix that ugly loop
            empty->type = JSON_OBJECT;
            empty->members = NULL; // initialize an empty object (point head to null)
            (*cursor)++; // go to next character
            skip_space(cursor); // skip any space before first member
            if(**cursor == '}')  // if the object closes immediately stop
            {
                (*cursor)++; // continue to the next thing
                return JSON_SUCCESS;
            }
            jmember* tail = empty->members; // points to NULL (because at first there is no tail)
            while(1) // go until object close
            {
                jmember* newMember = malloc(sizeof(jmember));
                if(newMember == NULL) return JSON_FAILURE;
                if(json_parse_member(cursor, newMember)) // try and parse in the next member
                {
                    newMember->next = NULL; // new element is going at the tail
                    if(tail == NULL) empty->members = newMember; // if there wasn't a tail, point the head to the new element
                    else tail->next = newMember; // if there was a tail, point it to the new element
                    tail = newMember; // set the tail to the new element
                }
                else return JSON_FAILURE;
                skip_space(cursor); // skip until the next thing
                if(**cursor == '}') break; // stop when encountering a closing bracket
                else if(**cursor != ',') return JSON_FAILURE; // fail when not finding a comma
                (*cursor)++; // increment the cursor
                skip_space(cursor);
            }
            (*cursor)++; // continue to the next thing
            return JSON_SUCCESS;

        case '[': // parse an array
            empty->type = JSON_ARRAY;
            int size = 4;
            empty->elements = calloc(size, sizeof(jvalue*)); // allocate space for 4 pointers (all set to null)
            if(empty->elements == NULL) return JSON_FAILURE;
            (*cursor)++;
            skip_space(cursor);
            if(**cursor == ']') // if array closes immediately, stop
            {
                (*cursor)++; // continue to the next thing
                return JSON_SUCCESS;
            }
            int i = 0;
            while(1) // TODO: also fix this ugly loop
            {
                jvalue* newValue = malloc(sizeof(jvalue));
                if(newValue == NULL) return JSON_FAILURE;
                if(json_parse_value(cursor, newValue)) // try to parse a value
                {
                    if(i + 1 == size) // are we about to overflow?
                    {
                        size *= 2; // double the size
                        jvalue** moreSpace = realloc(empty->elements, size * sizeof(jvalue*));
                        if(moreSpace == NULL) return JSON_FAILURE; // external caller should handle deallocation anyways
                        empty->elements = moreSpace; // have to twostep here so that external deallocation can find the old array in case of failure
                    }
                    empty->elements[i] = newValue;
                    i++;
                }
                else return JSON_FAILURE;
                skip_space(cursor); // skip until the next thing
                if(**cursor == ']') break; // stop when encountering a closing bracket
                else if(**cursor != ',') return JSON_FAILURE; // fail when not finding a comma
                (*cursor)++; // increment the cursor
                skip_space(cursor);
            }
            empty->elements[i] = NULL; // terminate the array
            jvalue** trimmed = realloc(empty->elements, (i + 1) * sizeof(jvalue*)); // free any unused space
            if(trimmed == NULL) return JSON_FAILURE;
            empty->elements = trimmed; // again twostep so caller can handle deallocation on failure
            (*cursor)++; // continue to the next thing
            return JSON_SUCCESS;

        case '"': // parse a string
            (*cursor)++;
            const char* start = *cursor;
            advance_to('"', cursor); // find the end of the string
            empty->string = calloc(*cursor - start + 1, 1); // allocate the string
            if(empty->string == NULL) return JSON_FAILURE;
            memcpy(empty->string, start, *cursor - start); // copy the string
            empty->type = JSON_STRING; // set object type
            (*cursor)++; // scoot the cursor past the end of the string (skip closing quotes)
            return JSON_SUCCESS; // done!

        case 't': // parse a true literal
            if(json_is_literal(cursor, "true")) // takes care of cursor movement
            {
                empty->type = JSON_BOOL;
                empty->boolean = 1;
                return JSON_SUCCESS;
            }
            return JSON_FAILURE;

        case 'f': // parse a false literal
            if(json_is_literal(cursor, "false")) // takes care of cursor movement
            {
                empty->type = JSON_BOOL;
                empty->boolean = 0;
                return JSON_SUCCESS;
            }
            return JSON_FAILURE;
        
        case 'n': // parse a null literal
            if(json_is_literal(cursor, "null")) // takes care of cursor movement
            {
                empty->type = JSON_NULL;
                return JSON_SUCCESS;
            }
            return JSON_FAILURE;

        default: // parse a number literal
            empty->type = JSON_NUMBER; // set type of the member
            const char before = **cursor; // save value at cursor FIXME: funky pointers!
            empty->number = strtod(*cursor, cursor); // strtod moves the cursor one past the last character interpreted
            return (empty->number == 0 && **cursor == before) ? JSON_FAILURE : JSON_SUCCESS; // TODO: make sure this check actually catches everything!
            // strtod returns 0 on failure, but does it really not move the cursor?
    }
}

jvalue* json_search_by_key(const char* key, const jvalue* obj)
{
    jmember* here = obj->members;
    while(here != NULL)
    {
        if(!strcmp(here->string, key)) return here->element;
        here = here->next;
    }
    return NULL;
}

// delete the first instance of a member with a certain key from an object
// returns JSON_FAILURE on failure, JSON_SUCCESS on success
int json_delete_first_member(const char* key, jvalue* obj)
{
    if(obj->type != JSON_OBJECT) return JSON_FAILURE;
    jmember* prev = NULL;
    jmember* curr = obj->members;
    while(curr != NULL && strcmp(key, curr->string) != 0) // as long as we're not at the end of the object and key and current string don't match
    {
        prev = curr; // continue on to the next object
        curr = curr->next;
    }
    if(curr == NULL) return JSON_SUCCESS; // nothing to delete
    if(prev == NULL) // have to delete the first member
        obj->members = curr->next;
    else
        prev->next = curr->next; // have the previous element skip over curr and point to the next element
    json_free_member(curr);
    return JSON_SUCCESS;
}

// delete all members with a certain key from an object
// returns JSON_FAILURE on failure, JSON_SUCCESS on success
int json_delete_all_members(const char* key, jvalue* obj)
{
    if(obj->type != JSON_OBJECT) return JSON_FAILURE;
    jmember* prev = NULL;
    jmember* curr = obj->members;
    while(curr != NULL) // go until end of object
    {
        if(!strcmp(key, curr->string))
        {
            if(prev == NULL) // have to delete first element
                obj->members = curr->next;
            else // have to delete a given element
                prev->next = curr->next;
            jmember* next = curr->next;
            json_free_member(curr);
            curr = next;
        }
        else
        {
            prev = curr; // advance to next element FIXME: this looks bugged
            curr = curr->next;
        }
    }
    return JSON_SUCCESS;
}

// add a member to an object (prepends)
// returns JSON_FAILURE on failure, JSON_SUCCESS on success
int json_add_member(const char* key, jvalue* element, jvalue* obj)
{
    if(obj->type != JSON_OBJECT) return JSON_FAILURE;
    jmember* new_member = malloc(sizeof(jmember));
    if(new_member == NULL) return JSON_FAILURE;
    new_member->string = calloc(strlen(key) + 1, 1);
    if(new_member->string == NULL)
    {
        free(new_member);
        return JSON_FAILURE;
    }
    strcpy(new_member->string, key);
    new_member->element = element;
    new_member->next = obj->members;
    obj->members = new_member;
    return JSON_SUCCESS;
}

// calculate how many characters are needed to print a json value - newlines and whitespace included (but not the null)
// newline after every opening bracket and every comma
// returns -1 on failure (null pointer)
static unsigned long jvallen(const jvalue* val)
{
    if(val == NULL) return -1;
    unsigned long length = -1;
    char buf[256];
    switch(val->type)
    {
        case JSON_OBJECT:
            length = 2; // {\n
            const jmember* now = val->members;
            while(now != NULL)
            {
                length += strlen(now->string) + 5 + jvallen(now->element) + (now->next != NULL ? 2 : 0); // "(string)" : (length of value) (,\n if there's another member coming)(5 + strlen + vallen + maybe 2)
                now = now->next; 
            }
            length += 2; // }\n
            break;
        case JSON_ARRAY:
            length = 2;
            for(int i = 0; val->elements[i] != NULL; i++)
                length += jvallen(val->elements[i]) + (val->elements[i + 1] != NULL ? 2 : 0); // same concept as for objects
            length += 2; // ]\n
            break;
        case JSON_STRING:
            length = strlen(val->string) + 2;
            break;
        case JSON_NUMBER:
            sprintf(buf, "%f", val->number);
            length = strlen(buf);
            break;
        case JSON_BOOL:
            length = val->boolean ? 4 : 5;
            break;
        case JSON_NULL:
            length = 4;
            break;

        default:
            break;
    }

    return length;
}

// remember to free what this returns!
char* jval_to_str(const jvalue* val)
{
    if(val == NULL) return NULL;
    char* out = calloc(jvallen(val) + 1, 1);
    if(out == NULL) return NULL;

    char* pos = out;
    switch(val->type)
    {
        case JSON_OBJECT:
            sprintf(pos, "{\n");
            pos += 2;
            jmember* now = val->members;
            while(now != NULL)
            {
                char* inner = jval_to_str(now->element); // TODO: this is probably slow - going over a lot of strings a lot of times.
                sprintf(pos, "\"%s\" : %s%s", now->string, inner, now->next != NULL ? ",\n" : "");
                pos += strlen(now->string) + 5 + strlen(inner) + (now->next != NULL ? 2 : 0);
                free(inner);
                now = now->next;
            }
            sprintf(pos, "\n}");
            pos += 2;
            break;
        case JSON_ARRAY:
            sprintf(pos, "[\n");
            pos += 2;
            for(int i = 0; val->elements[i] != NULL; i++)
            {
                char* inner = jval_to_str(val->elements[i]);
                sprintf(pos, "%s%s", inner, val->elements[i + 1] != NULL ? ",\n" : "");
                pos += strlen(inner) + (val->elements[i + 1] != NULL ? 2 : 0);
                free(inner);
            }
            sprintf(pos, "\n]");
            pos += 2;
            break;
        case JSON_STRING:
            sprintf(out, "\"%s\"", val->string);
            break;
        case JSON_NUMBER:
            sprintf(out, "%f", val->number);
            break;
        case JSON_BOOL:
            strcpy(out, val->boolean ? "true" : "false");
            break;
        case JSON_NULL:
            strcpy(out, "null");
            break;
        default:
            break;
    }
    return out;
}
