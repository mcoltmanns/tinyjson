#include <stdlib.h>
#include <string.h>

#include "parser.h"

/*
available tokens are:   punctuation ((curly) brackets, commas, colons)
                        strings
                        numbers
                        booleans
    use these as the terminals (denoted with _)

@json -> element

element -> object
        | array
        | string_
        | number_
        | bool_

object -> { }
        | { members }

members -> member
        | member , members

member -> string_ : element

array -> [ ]
        | [ elements ]

elements -> element
        | element , elements
*/
