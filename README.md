# tinyjson

A small C library for parsing JSON strings.
Designed to be as simple as possible - only two structure types, and the only non-parsing related functions are for searching by key in an object and printing a value to the terminal.
Written for my reimplementation of my reMarkable tablet synchronization utility.
## Getting going:
```
const char* test = "[1, 2, 3, true, false]";
const char** cursor = &test;
jvalue* v = malloc(sizeof(jvalue));
if(json_parse_value(cursor, v))
    json_print_value(v);
json_free_value(v);
```
`struct jvalue` holds a JSON value: object, array, string, number, boolean, or null

`struct jmember` holds a key-value pair where the key is a string, and the value is any `jvalue`

Better docs will arrive someday!
In the meantime, please check out my other stuff and let me know what I could be doing better :)
