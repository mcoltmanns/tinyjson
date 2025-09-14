# tinyjson

A small C library for parsing JSON strings.
Designed to be as simple as possible - only two structure types, and the only non-parsing related functions are for searching by key in an object and printing a value to the terminal.
Written for my reimplementation of my reMarkable tablet synchronization utility.

## Installation (system-wide)
- Clone the repository, then `$ cd ~/.../tinyjson`.
- `$ mkdir build`
- `$ cd build`
- `$ cmake ..`
- `$ make install`

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

## Manipulating JSON values
All JSON values are represented as a `jvalue`, which is a structured type with two fields: a type integer (`jvalue.type`) and a data value of variable type. See below:
| JSON type | `jvalue.type` | Data field type        | Data field name |
|-----------|---------------|------------        |----
| Object    | JSON_OBJECT   | `jmember* members` | `members`
| Array     | JSON_ARRAY    | `jvalue** elements`| `elements`
| String    | JSON_STRING   | `char* string`    | `string`
| Number    | JSON_NUMBER   | `double number`    | `number`
| Boolean   | JSON_BOOL     | `int boolean`      | `boolean`
| Null      | JSON_NULL     | undefined          | undefined

In an effort to keep the library light, data manipulation methods are not included (except for key search on objects). The following sections outline how each type of `jvalue` should be interacted with. Before accessing the data of any object, its type field should be checked.

### Objects
Objects are implemented as linked lists of `jmember` types. Each `jmember` contains a string (`jmember.string`), a pointer to a `jvalue` (`jmember.element`), and a pointer to the next member in the object (`jmember.next`).

Editing objects is as easy as moving pointers around to change the list. New members should be dynamically allocated.

### Arrays
Arrays are implemented as null-terminated arrays of pointers to `jvalues`. Accessing array elements can be done as you would in any other case, but resizing the array requires copy/reallocation.

### Strings
Strings are implemented as C-style (null-terminated) ASCII strings. During parsing, strings are read verbatim.

### Numbers
Numbers are implemented as doubles.

### Booleans
Booleans are implemented as integer values with positive logic (not 0 for true, 0 for false).

### Null values
Null values do not have a defined data field, and do not hold a value. Do not attempt to access the any data field of a null value.
