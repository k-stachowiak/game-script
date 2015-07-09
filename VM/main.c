#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "collection.h"
#include "memory.h"

#define REG_COUNT 16

#define ADDRESS_T uint64_t
#define BOOLEAN_T uint8_t
#define INTEGER_T int64_t
#define REAL_T double
#define CHARACTER_T uint8_t
#define CELL_T uint8_t
#define FLAG_T uint8_t

enum MoonOpCode {

    /* Memory manipulation */
    MOP_PUT,                 /* x <- value */
    MOP_PUT_LOCAL,
    MOP_PUT_VAL,
    MOP_PUT_ADDR,

    MOP_LOAD,                /* x <- memory */
    MOP_STORE,               /* x -> memory */
    MOP_PUSH,                /* x -> stack */
    MOP_POP,                 /* x <- stack */

    /* Data processing */
    MOP_INCR,                /* Rx++ */
    MOP_DECR,                /* Rx-- */
    MOP_NOT,                 /* Rx = -Rx */
    MOP_ADD,                 /* R0 += R1 */
    MOP_SUB,                 /* R0 -= R1 */
    MOP_MUL,                 /* R0 *= R1 */
    MOP_DIV,                 /* R0 /= R1 */
    MOP_MOD,                 /* R0 %= R1 */
    MOP_AND,                 /* R0 &= R1 */
    MOP_OR,                  /* R0 |= R1 */
    MOP_XOR,                 /* R0 ^= R1 */

    /* Execution control */
    MOP_JUMP,                /* IP = R0 */
    MOP_JUMP_EQUAL,          /* if ZF then R0 -> IP */
    MOP_JUMP_NOT_EQUAL,      /* if !ZF then R0 -> IP */
    MOP_JUMP_GREATER,        /* if !ZF or SF = OF then R0 -> IP */
    MOP_JUMP_GREATER_EQUAL,  /* if SF = OF then R0 -> IP */
    MOP_JUMP_LESS,           /* if SF != OF then R0 -> IP */
    MOP_JUMP_LESS_EQUAL,     /* if ZF or SF != OF then R0 -> IP*/
    MOP_TERMINATE,           /* end execution */

    /* I/O */
    MOP_OUT,                 /* R0 -> out */
    MOP_IN,                  /* R0 <- in */

    MOP_MAX

};

#if MOP_MAX >= 256
#   error Too many op codes to fit in a single byte
#endif

enum MoonVmValueType {
};

struct MoonVmValue {
    MoonVmValueType type;
    union {
        ADDRESS_T addr;
        BOOLEAN_T boolean;
        INTEGER_T integer;
        REAL_T real;
        CHARACTER_T character;
    } data;
};

struct MoonVm {

    struct MoonVmValue registers[REG_COUNT];

    FLAG_T zero_flag;
    FLAG_T sign_flag;
    FLAG_T overflow_flag;

    ADDRESS_T instruction_pointer;

    struct {
        CELL_T *data;
        int size, cap;
    } stack;
};

enum MoonVmResource {
    MVR_ZF = REG_COUNT,
    MVR_SF,
    MVR_OF
};

static CELL_T vm_next_cell(struct MoonVm *vm)
{
    return vm->stack.data + vm->instruction_pointer++;
}

static void vm_next_n(struct MoonVm *vm, CELL_T* dst, int n)
{
    while (n--) *dst++ = mn_next_cell;
}

static bool vm_is_reg(CELL_T resource)
{
    return resource > 0 && resource < REG_COUNT;
}

static void vm_op_put(struct MoonVm *vm)
{
    CELL_T dst_id = vm_next_cell(vm);
    CELL_T src_type = vm_next_cell(vm);

    char *dst;
    int dst_size;

    char *src;
    int src_size;

    if (vm_is_reg(dst)) {
        dst = vm->registers + dst_id;

    } else {
        switch (dst) {
        case MVR_ZF:
            dst = &vm->zero_flag;
            break;
        case MVR_SF:
            dst = &vm->sign_flag;
            break;
        case MVR_OF:
            dst = &vm->overflow_flag;
            break;
        }
    }


}

static bool vm_step(struct MoonVm *vm)
{
    int resource_id;

    switch (vm_next_cell(vm)) {

    case MOP_PUT:
        vm_op_put(vm);
		break;

    case MOP_LOAD:
		break;

    case MOP_STORE:
		break;

    case MOP_PUSH:
		break;

    case MOP_POP:
		break;

    case MOP_INCR:
		break;

    case MOP_DECR:
		break;

    case MOP_NOT:
		break;

    case MOP_ADD:
		break;

    case MOP_SUB:
		break;

    case MOP_MUL:
		break;

    case MOP_DIV:
		break;

    case MOP_MOD:
		break;

    case MOP_AND:
		break;

    case MOP_OR:
		break;

    case MOP_XOR:
		break;

    case MOP_JUMP:
		break;

    case MOP_JUMP_EQUAL:
		break;

    case MOP_JUMP_NOT_EQUAL:
		break;

    case MOP_JUMP_GREATER:
		break;

    case MOP_JUMP_GREATER_EQUAL:
		break;

    case MOP_JUMP_LESS:
		break;

    case MOP_JUMP_LESS_EQUAL:
		break;

    case MOP_TERMINATE:
		break;

    case MOP_OUT:
		break;

    case MOP_IN:
		break;
    }

    return true;
}

void vm_init(struct MoonVm *vm)
{
    memset(vm, 0, sizeof(*vm));
}

void vm_deinit(struct MoonVm *vm)
{
    ARRAY_FREE(vm.stack);
    memset(vm, 0, sizeof(*vm));
}

void vm_load(struct MoonVm *vm, CELL_T *data, int size)
{
    while (size--) ARRAY_APPEND(vm->stack, data++);
}

void vm_run(struct MoonVm *vm)
{
    while (vm->stack.data[vm->instruction_pointer] != MOP_TERMINATE) {
        if (!vm_step(vm)) {
            printf("error\n");
            break;
        }
    }
};

int main()
{
    struct MoonVm vm;
    vm_init(&vm);
    vm_deinit(&vm);
	return 0;
}

