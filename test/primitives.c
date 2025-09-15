//
// Created by moltmanns on 9/15/25.
// Primitive parsing tests (null, booleans, numbers)
// For absolute best coverage run with valgrind
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tinyjson.h"

void run_test(int (*test_func)(int), char* name, const int verbose) {
    printf("Running test \"%s\"...\n", name);
    int result = test_func(verbose);
    printf(result ? "failed (%d)\n" : "passed (%d)\n", result);
}

int null_test(const int verbose) {
    // positive test
    char* in = " null ";
    jvalue* json = calloc(1, sizeof(jvalue));
    if (json_parse_value(&in, json) != JSON_SUCCESS) {
        json_free_value(json);
        if (verbose) {
            printf("JSON_PARSE_VALUE failed (%s)\n", in);
        }
        return 1;
    }
    if (json->type != JSON_NULL) {
        if (verbose) {
            printf("Parsed value has wrong type\n");
        }
        json_free_value(json);
        return 1;
    }
    char* out = json_jval_to_str(json);
    if (strcmp(out, "null") != 0) {
        json_free_value(json);
        free(out);
        if (verbose) {
            printf("Output comparison failed\n");
        }
        return 1;
    }
    json_free_value(json);
    free(out);
    return 0;
}

int bool_true_test(const int verbose) {
    // true test
    char* in = " true ";
    jvalue* json = calloc(1, sizeof(jvalue));
    if (json_parse_value(&in, json) != JSON_SUCCESS) {
        json_free_value(json);
        if (verbose) {
            printf("JSON_PARSE_VALUE failed (%s)\n", in);
        }
        return 1;
    }
    if (json->type != JSON_BOOL) {
        if (verbose) {
            printf("Parsed value has wrong type\n");
        }
        json_free_value(json);
        return 1;
    }
    if (json->boolean != 1) {
        if (verbose) {
            printf("Parsed value is incorrect\n");
        }
        json_free_value(json);
        return 1;
    }
    char* out = json_jval_to_str(json);
    if (strcmp(out, "true") != 0) {
        json_free_value(json);
        free(out);
        if (verbose) {
            printf("Output comparison failed\n");
        }
        return 1;
    }
    json_free_value(json);
    free(out);
    return 0;
}

int bool_false_test(const int verbose) {
    // false test
    char* in = "false ";
    jvalue* json = calloc(1, sizeof(jvalue));
    if (json_parse_value(&in, json) != JSON_SUCCESS) {
        json_free_value(json);
        if (verbose) {
            printf("JSON_PARSE_VALUE failed (%s)\n", in);
        }
        return 1;
    }
    if (json->type != JSON_BOOL) {
        if (verbose) {
            printf("Parsed value has wrong type\n");
        }
        json_free_value(json);
        return 1;
    }
    if (json->boolean != 0) {
        if (verbose) {
            printf("Parsed value is incorrect\n");
        }
        json_free_value(json);
        return 1;
    }
    char* out = json_jval_to_str(json);
    if (strcmp(out, "false") != 0) {
        json_free_value(json);
        free(out);
        if (verbose) {
            printf("Output comparison failed\n");
        }
        return 1;
    }
    json_free_value(json);
    free(out);
    return 0;
}

int numbers_int_test(const int verbose) {
    // integer
    char* in = " 42";
    jvalue* json = calloc(1, sizeof(jvalue));
    if (json_parse_value(&in, json) != JSON_SUCCESS) {
        json_free_value(json);
        if (verbose) {
            printf("JSON_PARSE_VALUE failed (%s)\n", in);
        }
        return 1;
    }
    if (json->type != JSON_NUMBER) {
        if (verbose) {
            printf("Parsed value has wrong type\n");
        }
        json_free_value(json);
        return 1;
    }
    if (json->number != 42) {
        if (verbose) {
            printf("Parsed value is incorrect\n");
        }
        json_free_value(json);
        return 1;
    }
    char* out = json_jval_to_str(json);
    if (strcmp(out, "42") != 0) {
        if (verbose) {
            printf("Output comparison failed\n");
        }
        json_free_value(json);
        free(out);
        return 1;
    }
    json_free_value(json);
    free(out);
    return 0;
}

int numbers_negative_test(const int verbose) {
    // negative
    char* in = "-7";
    jvalue* json = calloc(1, sizeof(jvalue));
    if (json_parse_value(&in, json) != JSON_SUCCESS) {
        json_free_value(json);
        if (verbose) {
            printf("JSON_PARSE_VALUE failed (%s)\n", in);
        }
        return 1;
    }
    if (json->type != JSON_NUMBER) {
        if (verbose) {
            printf("Parsed value has wrong type\n");
        }
        json_free_value(json);
        return 1;
    }
    if (json->number != -7) {
        if (verbose) {
            printf("Parsed value is incorrect\n");
        }
        json_free_value(json);
        return 1;
    }
    char* out = json_jval_to_str(json);
    if (strcmp(out, "-7") != 0) {
        if (verbose) {
            printf("Output comparison failed\n");
        }
        json_free_value(json);
        free(out);
        return 1;
    }
    json_free_value(json);
    free(out);
    return 0;
}

int numbers_fp_test(const int verbose) {
    char* in = "3.1415";
    jvalue* json = calloc(1, sizeof(jvalue));
    if (json_parse_value(&in, json) != JSON_SUCCESS) {
        json_free_value(json);
        if (verbose) {
            printf("JSON_PARSE_VALUE failed (%s)\n", in);
        }
        return 1;
    }
    if (json->type != JSON_NUMBER) {
        if (verbose) {
            printf("Parsed value has wrong type\n");
        }
        json_free_value(json);
        return 1;
    }
    if (json->number != 3.1415) {
        if (verbose) {
            printf("Parsed value is incorrect\n");
        }
        json_free_value(json);
        return 1;
    }
    char* out = json_jval_to_str(json);
    if (strcmp(out, "3.1415") != 0) {
        if (verbose) {
            printf("Output comparison failed\n");
        }
        json_free_value(json);
        free(out);
        return 1;
    }
    json_free_value(json);
    free(out);
    return 0;
}

int numbers_exp_test(const int verbose) {
    char* in = "1e10";
    jvalue* json = calloc(1, sizeof(jvalue));
    if (json_parse_value(&in, json) != JSON_SUCCESS) {
        json_free_value(json);
        if (verbose) {
            printf("JSON_PARSE_VALUE failed (%s)\n", in);
        }
        return 1;
    }
    if (json->type != JSON_NUMBER) {
        if (verbose) {
            printf("Parsed value has wrong type\n");
        }
        json_free_value(json);
        return 1;
    }
    if (json->number != 1e10) {
        if (verbose) {
            printf("Parsed value is incorrect\n");
        }
        json_free_value(json);
        return 1;
    }
    char* out = json_jval_to_str(json);
    if (strcmp(out, "1e10") != 0) {
        if (verbose) {
            printf("Output comparison failed\n");
        }
        json_free_value(json);
        free(out);
        return 1;
    }
    json_free_value(json);
    free(out);
    return 0;
}

int numbers_zeros_test(const int verbose) {
    char* ins[] = {"0", "0.0", "-0"};
    for (int i = 0; i < 3; i++) {
        char* in = ins[i];
        jvalue* json = calloc(1, sizeof(jvalue));
        if (json_parse_value(&in, json) != JSON_SUCCESS) {
            json_free_value(json);
            if (verbose) {
                printf("JSON_PARSE_VALUE failed (%s)\n", in);
            }
            return 1;
        }
        if (json->type != JSON_NUMBER) {
            if (verbose) {
                printf("Parsed value has wrong type\n");
            }
            json_free_value(json);
            return 1;
        }
        if (json->number != 0) {
            if (verbose) {
                printf("Parsed value is incorrect\n");
            }
            json_free_value(json);
            return 1;
        }
        char* out = json_jval_to_str(json);
        if (strcmp(out, ins[i]) != 0) {
            if (verbose) {
                printf("Output comparison failed\n");
            }
            json_free_value(json);
            free(out);
            return 1;
        }
        json_free_value(json);
        free(out);
    }
    return 0;
}

int main(int argc, char **argv) {
    const int verbose = 1;
    printf("Primitives\n");
    run_test(null_test, "null", verbose);
    run_test(bool_true_test, "boolean_true", verbose);
    run_test(bool_false_test, "boolean_false", verbose);
    run_test(numbers_int_test, "numbers_int", verbose);
    run_test(numbers_negative_test, "numbers_negative", verbose);
    run_test(numbers_fp_test, "numbers_fp", verbose);
    run_test(numbers_exp_test, "numbers_exp", verbose);
    run_test(numbers_zeros_test, "numbers_zeros", verbose);
    return 0;
}
