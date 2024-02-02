#include <stdio.h>
#include <stdlib.h>

enum json_err
{
    JSON_BAD_TYPE,
    JSON_NO_KEY
};

enum json_types
{
    OBJECT_T,
    ARRAY_T,
    STRING_T,
    NUMBER_T,
    BOOL_T,
    KEYVAL_T,
    NULL_T,
    TERM_T // terminator for arrays and objects (like \0 for strings)
};

typedef struct json_keyval json_keyval;
typedef struct json_value json_value;

struct json_keyval // holds a key and a value - keys within an object should be unique! duplicates are undefined behavior
{
    char* key;
    json_value* value;
};

struct json_value
{
    int type;
    union
    { // can be either an object, array, string, number, bool, or null
        char* string;
        double number;
        int boolean;
        json_keyval* object; // pointer to the beginning of a memory sequence where the different key-value pairs are kept
        json_value* array; // pointer to an array of json objects. remember to malloc and free! and watch your indexes
    };
};

/*
object and array access are complicated.
for objects: want to be able to search by key, and return the keyval that matches that key
for arrays: want to be able to access by index
*/

// returns 0 on success
int json_search_object(const char* key, const json_value* obj, json_keyval* out)
{
    if(obj->type != OBJECT_T) return JSON_BAD_TYPE;

    json_keyval* curr_obj = obj->object;
    while(*(curr_obj->key) != *key)
        curr_obj++;

    return 0;
}
