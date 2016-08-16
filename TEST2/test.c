#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "term.h"
#include "parse.h"
#include "error.h"
#include "runtime.h"
#include "collection.h"

#define COMMENT_CHAR '#'

static struct Runtime *rt;
bool last_success;
VAL_LOC_T last_loc;
char *current_test_name;
int tests_performed, tests_failed;

#define report_failure(FORMAT, ...) \
    do { \
        printf("[FAIL] " FORMAT "\n", ##__VA_ARGS__); \
        ++tests_failed; \
    } while(0)

void str_tokenize(char *string, char ***tokens, int *token_count)
{
    /* 1. Initialize the result object. */
    struct {
        char **data;
        int size, cap;
    } result = { NULL, 0, 0 };

    /* 2. Iterate over the tokens in the string. */
    while (*string) {

        /* 2.1. Find non-space character or end of string. */
        while (*string && isspace(*string)) {
            ++string;
        }

        /* 2.2. All spaces until the end of string - no more tokens. */
        if (*string == '\0') {
            goto end;
        }

        /* 2.3. Check if quoted token or a regular one. */
        if (*string == '"') {

            /* 2.3.b.1. Store token beginning address. */
            ARRAY_APPEND(result, ++string);

            /* 2.3.b.2. Find terminating quote or end of string. */
            while (*string && *string != '"') {
                ++string;
            }

            /* 2.3.b.3 End of string reached? No more tokens. */
            if (*string == '\0') {
                fprintf(stderr, "End of string while looking for terminating quote\n");
                exit(1);
            }

            /* 2.3.b.4. If not at the end of string, end token with synthethic NUL. */
            *string = '\0';

        } else {

            /* 2.3.b.1. Store token beginning address. */
            ARRAY_APPEND(result, string);

            /* 2.3.b.2. Find space character or end of string. */
            while (*string && !isspace(*string)) {
                ++string;
            }

            /* 2.3.b.3 End of string reached? No more tokens. */
            if (*string == '\0') {
                goto end;
            }

            /* 2.3.b.4. If not at the end of string, end token with synthethic NUL. */
            *string = '\0';
        }

        /* 2.4. Next char. */
        ++string;
    }
end:
    *tokens = result.data;
    *token_count = result.size;
}

bool is_comment(char *line)
{
    while (*line && *line != COMMENT_CHAR) {
        ++line;
    }
    return *line == COMMENT_CHAR;
}

bool is_empty(char *line)
{
    while (*line && isspace(*line)) {
        ++line;
    }
    return *line == '\0';
}

void run_command_test(char *args)
{
    int len = strlen(args);

    mem_free(current_test_name);
    rt_free(rt);

    rt = rt_make();
    current_test_name = (char*)mem_malloc(len + 1);
    memcpy(current_test_name, args, len + 1);
    last_success = false;
}

void run_command_expect(char *args)
{
    char **tokens;
    int count;

    ++tests_performed;

    str_tokenize(args, &tokens, &count);

    if (count == 1) {

        if (strcmp(tokens[0], "SUCCESS") == 0) {
            if (!last_success) {
                report_failure("Expected success, but failure encountered");
            }
        } else if (strcmp(tokens[0], "FAILURE") == 0) {
            if (last_success) {
                report_failure("Expected failure, but success encountered");
            }
        } else {
            fprintf(stderr, "Unexpected single EXPECT argument: %s", tokens[0]);
            exit(1);
        }

    } else if (count == 2) {

        char *type_string = tokens[0];
        char *value_string = tokens[1];

        if (strcmp(type_string, "bool") == 0) {

            if (rt_val_peek_type(&rt->stack, last_loc) != VAL_BOOL) {
                report_failure("Expected boolean result type, but other encountered");
            } else if (strcmp(value_string, "true") == 0) {
                if (!rt_val_peek_bool(rt, last_loc)) {
                    report_failure("Expected true value, but false encountered");
                }
            } else if (strcmp(value_string, "false") == 0) {
                if (rt_val_peek_bool(rt, last_loc)) {
                    report_failure("Expected false value, but true encountered");
                }
            } else {
                fprintf(stderr, "Unexpected bool literal: %s", value_string);
                exit(1);
            }

        } else if (strcmp(type_string, "int") == 0) {

            if (rt_val_peek_type(&rt->stack, last_loc) != VAL_INT) {
                report_failure("Expected integer result type, but other encountered");
            } else {
                int expected = (int)strtol(value_string, (char**)NULL, 10);
                int actual = rt_val_peek_int(rt, last_loc);
                if (expected != actual) {
                    report_failure("Expected %d, but %d encountered", expected, actual);
                }
            }

        } else if (strcmp(type_string, "real") == 0) {

            if (rt_val_peek_type(&rt->stack, last_loc) != VAL_REAL) {
                report_failure("Expected real result type, but other encountered");
            } else {
                double expected = strtod(value_string, (char**)NULL);
                double actual = rt_val_peek_real(rt, last_loc);
                if (expected != actual) {
                    report_failure("Expected %f, but %f encountered", expected, actual);
                }
            }

        } else if (strcmp(type_string, "char") == 0) {

            if (rt_val_peek_type(&rt->stack, last_loc) != VAL_CHAR) {
                report_failure("Expected character result type, but other encountered");
            } if (strlen(value_string) != 1) {
                fprintf(stderr, "Incorrect value for char expectation: %s", value_string);
                exit(1);
            } else {
                char expected = value_string[0];
                char actual = rt_val_peek_char(rt, last_loc);
                if (expected != actual) {
                    report_failure("Expected %c, but %c encountered", expected, actual);
                }
            }

        } else if (strcmp(type_string, "string") == 0) {

            if (!rt_val_is_string(rt, last_loc)) {
                report_failure("Expected string result type, but other encountered");
            } else {
                char *actual = rt_val_peek_cpd_as_string(rt, last_loc);
                char *expected = value_string;
                if (strcmp(expected, actual) != 0) {
                    report_failure("Expected %s, but %s encountered", expected, actual);
                }
                mem_free(actual);
            }

        } else {
            fprintf(stderr, "Unexpected EXPECT type argument: %s", type_string);
            exit(1);
        }

    } else {
        fprintf(stderr, "Incorrect arguments count to EXPECT: %d", count);
        exit(1);
    }
}

bool run_command(char *line)
{
    int len;
    char *copy;
    char *current, *command, *args;

    len = strlen(line);
    copy = (char*)mem_malloc(len + 1);
    memcpy(copy, line, len + 1);

    current = copy;

    while (*current && !isspace(*current)) {
        ++current;
    }

    if (*current == '\0') {
        mem_free(copy);
        return false;
    } else {
        command = copy;
        *current = '\0';
        do {
            ++current;
        } while (*current && isspace(*current));
        args = current;
    }

    if (strcmp(command, "TEST") == 0) {
        run_command_test(args);
        return true;
    } else if (strcmp(command, "EXPECT") == 0) {
        run_command_expect(args);
        return true;
    } else {
        return false;
    }
}

bool parse_line(char *line)
{
    struct AstNode *node;

    err_reset();
    node = parse_source(line, NULL, NULL);

    if (!node) {
        last_success = false;
        return false;
    }

    if (!rt_consume_list(rt, node, NULL, &last_loc)) {
        last_success = false;
        return false;
    }

    last_success = true;
    return true;
}

bool run_script(FILE *script)
{
    bool eof = false;
    rt = rt_make();
    last_success = false;
    tests_performed = tests_failed = 0;
    while (!eof) {
        char *line = my_getline(script, &eof);
        is_empty(line) ||
            is_comment(line) ||
            run_command(line) ||
            parse_line(line);
        mem_free(line);
    }
    mem_free(current_test_name);
    rt_free(rt);
    printf("Tests summary: %d/%d failed\n", tests_failed, tests_performed);
    return true;
}

void x(void)
{
    printf("Capturing program ending, because it's beyond Visual Studio 2015\n");
}

int main(int argc, char *argv[])
{
    atexit(x);

    if (argc != 2) {
        fprintf(stderr, "Usage: %s {test-script}\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *script = fopen(argv[1], "r");
    if (!script) {
        fprintf(stderr, "Failed loading script file: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    if (!run_script(script)) {
        fprintf(stderr, "Failed running test script\n");
        fclose(script);
        return EXIT_FAILURE;
    }

    fclose(script);
    return EXIT_SUCCESS;
}