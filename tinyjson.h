#ifndef HEADER
#define HEADER

#define JSON_SUCCESS 1
#define JSON_FAILURE 0

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

// free the memory associated with a jvalue
// also frees all members/elements of the value if it's an array or object
void json_free_value(jvalue* v);

// parse a json value from a string, and leave the cursor on the first character after that value
// cursor should be the address of the beginning of a string
// empty should be a dynamically allocated jvalue (will be filled)
// returns JSON_FAILURE on fail (due to syntax or memory errors)
// regardless of success or failure, the caller is expected to allocate and free empty
int json_parse_value(const char** cursor, jvalue* empty);

// print a json value
// output is not guaranteed to be valid json!
void json_print_value(const jvalue* v);

// search for a certain key in a json object (non-recursive)
// returns NULL if the key didn't exist, returns a pointer to the value associated with the first instance of the key otherwise
// caller should ensure the jvalue being passed is a properly built object!
jvalue* json_search_by_key(const char* key, const jvalue* obj);

#endif
