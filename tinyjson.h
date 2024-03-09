#ifndef HEADER
#define HEADER

#define JSON_SUCCESS 1
#define JSON_FAILURE 0

enum JSON_TYPES {
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

// print a json value to stdout (uses printf)
void json_print_value(jvalue* v);

// free the memory associated with a jvalue
// also frees all members/elements of the value if it's an array or object
void json_free_value(jvalue* v);

// parse a json value from a string, and leave the cursor on the first character after that value
// cursor should be the address of the beginning of a string (eg char** cursor = &str)
// empty should be a previously malloc'd jvalue (will be filled)
// returns JSON_FAILURE on fail (due to syntax or memory errors)
// regardless of success or failure, the caller is expected to allocate and free empty (using json_free_value)
int json_parse_value(const char** cursor, jvalue* empty);

// search for a certain key in a json object (non-recursive)
// returns NULL if the key didn't exist, returns a pointer to the value associated with the first instance of the key otherwise
// caller should ensure the jvalue being passed is a properly built object!
jvalue* json_search_by_key(const char* key, const jvalue* obj);

// allocate and return a pointer to a valid json string representing val
// output will be valid json, but not necessarily pretty
// returns NULL on failure
char* jval_to_str(const jvalue* val);

#endif
