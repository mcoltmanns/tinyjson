# tinyjson

A small C library for parsing JSON strings.
Designed to be as simple as possible - only two structure types, and the only non-parsing related functions are for searching by key in an object and printing a value to the terminal.
Written for my reimplementation of my reMarkable tablet synchronization utility.
## Getting going:
```
const char* data = "{\"key\" : \"value\", \"array\" : [true, false, null] }";
jvalue* val = malloc(sizeof(jvalue));
if(json_parse_value(&data, val))
{
    char* str = jval_to_str(val);
    printf("%s\n", str);
    free(str);
}
json_free_value(val);
```
`struct jvalue` holds a JSON value: object, array, string, number, boolean, or null

`struct jmember` holds a key-value pair where the key is a string, and the value is any `jvalue`

I try to keep things legible and commented, so questions should be answerable by reading the source. But if you still have questions (or suggestions!), feel free to ask.

In the meantime, please check out my other stuff and let me know what I could be doing better :)
