#include "tstream.h"

enum element_type {
    OBJECT,
    ARRAY,
    STRING,
    NUMBER,
    BOOLEAN
};

typedef struct element element;
typedef struct member member;

struct element { // elements can be
    int type; // (used internally)
    union {
        member* object; // an array of members (an object)
        element* array; // an array of elements (an array)
        char* string; // a string
        double number; // a number
        int boolean; // a boolean
    };
};

struct member {
    char* string;
    element element;
};
