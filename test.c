#include "tinyjson.c"

void main()
{
    const char* str = "{ \"one\": { \"key\" : \"value\" }, \"two\": \"2\", \"boolean\": true, \"array\" : [1, 2, 3] }";
    const char** cursor = &str;
    jvalue* val = malloc(sizeof(jvalue));
    if(json_parse_value(cursor, val))
        json_print_value(val);
    else printf("no good");
    printf("\n");
    json_free_value(val);
}