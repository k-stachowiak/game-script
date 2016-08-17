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
int unexpected_fails;
VAL_LOC_T last_loc;
char *current_test_name;
char *last_expression;
int tests_performed, tests_failed;

#define report_failure(FORMAT, ...) \
    do { \
        printf("[FAIL][%s][%s] " FORMAT "\n", current_test_name, last_expression, ##__VA_ARGS__); \
        ++tests_failed; \
    } while(0)

char *type_to_string(enum ValueType type)
{
    switch (type) {
    case VAL_BOOL:
        return "bool";
    case VAL_INT:
        return "int";
    case VAL_REAL:
        return "real";
    case VAL_CHAR:
        return "char";
    case VAL_ARRAY:
        return "array";
    case VAL_TUPLE:
        return "tuple";
    case VAL_FUNCTION:
        return "function";
    case VAL_PTR:
        return "pointer";
    case VAL_UNIT:
        return "unit";
    case VAL_DATATYPE:
        return "datatype";
    }
}

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

    mem_free(last_expression);
    mem_free(current_test_name);
    rt_free(rt);

    rt = rt_make();
    current_test_name = (char*)mem_malloc(len);
    memcpy(current_test_name, args, len - 1);
    current_test_name[len - 1] = '\0';
    last_expression = NULL;
    unexpected_fails = 0;
}

void run_command_expect(char *args)
{
    char **tokens;
    int count;

    ++tests_performed;

    str_tokenize(args, &tokens, &count);

    if (count == 1) {

        if (strcmp(tokens[0], "SUCCESS") == 0) {
            if (unexpected_fails) {
                report_failure(
                    "Expected success, but %d unexpected failure(s) encountered",
                    unexpected_fails);
            }
        } else if (strcmp(tokens[0], "FAILURE") == 0) {
            if (!unexpected_fails) {
                report_failure("Expected failure, but none encountered");
            } else {
                --unexpected_fails;
            }
        } else {
            fprintf(stderr, "Unexpected single EXPECT argument: %s", tokens[0]);
            exit(1);
        }

    } else if (count == 2) {

        char *type_string = tokens[0];
        char *value_string = tokens[1];

        if (unexpected_fails) {
            report_failure(
                "%d unexpected failure(s) encountered before an expect check",
                unexpected_fails);
        } else if (strcmp(type_string, "bool") == 0) {

            enum ValueType last_type = rt_val_peek_type(&rt->stack, last_loc);
            if (last_type != VAL_BOOL) {
                report_failure(
                    "Expected boolean result type, but %s encountered",
                    type_to_string(last_type));
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

            enum ValueType last_type = rt_val_peek_type(&rt->stack, last_loc);
            if (last_type != VAL_INT) {
                report_failure(
                    "Expected integer result type, but %s encountered",
                    type_to_string(last_type));
            } else {
                int expected = (int)strtol(value_string, (char**)NULL, 10);
                int actual = rt_val_peek_int(rt, last_loc);
                if (expected != actual) {
                    report_failure("Expected %d, but %d encountered", expected, actual);
                }
            }

        } else if (strcmp(type_string, "real") == 0) {

            enum ValueType last_type = rt_val_peek_type(&rt->stack, last_loc);
            if (last_type != VAL_REAL) {
                report_failure(
                    "Expected real result type, but %s encountered",
                    type_to_string(last_type));
            } else {
                double expected = strtod(value_string, (char**)NULL);
                double actual = rt_val_peek_real(rt, last_loc);
                if (expected != actual) {
                    report_failure("Expected %f, but %f encountered", expected, actual);
                }
            }

        } else if (strcmp(type_string, "char") == 0) {

            enum ValueType last_type = rt_val_peek_type(&rt->stack, last_loc);
            if (last_type != VAL_CHAR) {
                report_failure(
                    "Expected character result type, but %s encountered",
                    type_to_string(last_type));
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

            enum ValueType last_type = rt_val_peek_type(&rt->stack, last_loc);
            if (!rt_val_is_string(rt, last_loc)) {
                report_failure(
                    "Expected string result type, but %s encountered",
                    type_to_string(last_type));
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
    int len = strlen(line);

    err_reset();
    node = parse_source(line, NULL, NULL);

    if (!node) {
        ++unexpected_fails;
        return false;
    }

    if (!rt_consume_list(rt, node, NULL, &last_loc)) {
        ++unexpected_fails;
        return false;
    }

    mem_free(last_expression);
    last_expression = mem_malloc(len - 1);
    memcpy(last_expression, line, len - 2);
    last_expression[len - 2] = '\0';

    return true;
}

bool run_script(FILE *script)
{
    bool eof = false;
    rt = rt_make();
    unexpected_fails = 0;
    tests_performed = tests_failed = 0;
    last_expression = NULL;
    while (!eof) {
        char *line = my_getline(script, &eof);
        is_empty(line) ||
            is_comment(line) ||
            run_command(line) ||
            parse_line(line);
        mem_free(line);
    }
    mem_free(last_expression);
    mem_free(current_test_name);
    rt_free(rt);
    printf(
        "Summary:\n"
        "* %d/%d tests succeeded,\n"
        "* %d pending unexpected failures.\n",
        tests_performed - tests_failed,
        tests_performed,
        unexpected_fails);
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