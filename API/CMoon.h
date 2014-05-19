#ifndef MOON_API_CMOON_H
#define MOON_API_CMOON_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Error related.
 * ==============
 */

enum mnError {
    MN_OK
};

/* Value related.
 * ==============
 */

enum mnValueType {
    MN_INTEGER,
    MN_REAL,
    MN_CHAR,
    MN_STRING,
    MN_BOOL
};

struct mnValue {
    enum mnValueType type;
    union {
        long integer;
        double real;
        char character;
        int boolean;
        char *string;
        struct mnValue *compound;
    } body;
    int size; /* For string and compound only. */
};

void mnDestroyValue(struct mnValue value);
enum mnError mnCreateInteger(long value, struct mnValue *result);
enum mnError mnCreateReal(double value, struct mnValue *result);
enum mnError mnCreateCharacter(char value, struct mnValue *result);
enum mnError mnCreateBoolean(int value, struct mnValue *result);
enum mnError mnCreateString(char *value, struct mnValue *result);
enum mnError mnCreateCompound(struct mnValue *compound, int size, struct mnValue *result);

/* Interpreter related.
 * ====================
 */

typedef int mnItprId;

ItprId mnCreateInterpreter(void);
enum mnError mnItprLoadUnitFile(ItprId itpr, char *filename);
enum mnError mnItprLoadUnitString(ItprId itpr, char *source);
enum mnError mnItprGetAllValueSymbols(ItprId itpr, char **symbols, int numSymbols);
enum mnError mnItprGetAllFunctionSymbols(ItprId itpr, char **symbols, int numSymbols);
enum mnError mnItprGetValue(ItprId itpr, char *unit, char *symbol, mnValueId *result);
enum mnError mnItprCallFunction(ItprId itpr, char *unit, char *symbol, mnValueId *args, int numArgs, mnValueId *result);
void mnDestroyInterpreter(ItprId itpr);

#ifdef __cplusplus
}
#endif

#endif
