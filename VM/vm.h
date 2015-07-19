/* Copytight (C) 2015 Krzysztof Stachowiak */

#ifndef VM_H
#define VM_H

#include <stdbool.h>
#include <stdint.h>

#define REG_COUNT 16

#define REG_T int64_t
#define ADDRESS_T uint64_t
#define BOOLEAN_T uint8_t
#define INTEGER_T int64_t
#define REAL_T double
#define CHARACTER_T uint8_t
#define CELL_T uint8_t
#define FLAG_T uint8_t

/* Resources
 *
 * register ::= r0 | r1 | r2 | ... | REG_COUNT - 1
 * flag ::= zero flag | sign flag | overflow flag
 * pointer ::= instruction pointer | stack pointer
 *
 * local ::= register | flag | pointer
 * literal ::= 1B | 2B | 4B | 8B | IEEE
 * named_resource ::= local | derefered
 * resource ::= named_resource | literal
 */

enum MoonResCode {

	/* Resource identification
	 *
	 * local resources are given by the numbers 0 .. LOCAL_RES_COUNT; this spans:
	 * 0 .. REG_COUNT - 1 : registers
	 * REG_COUNT .. REG_COUNT + FLAG_COUNT - 1 : flags
	 * REG_COUNT + FLAG_COUNT .. REG_COUNT + FLAG_COUNT + PTR_COUNT - 1 : pointers
	 *
	 * therefore REG_COUNT + FLAG_COUNT + PTR_COUNT = LOCAL_RES_COUNT
	 */

	MR_R0, MR_R1, MR_R2, MR_R3,
	MR_R4, MR_R5, MR_R6, MR_R7,
	MR_R8, MR_R9, MR_R10, MR_R11,
	MR_R12, MR_R13, MR_R14, MR_R15,

	MR_ZF, MR_SF, MR_OF, MR_PF, MR_CF,
	MR_IP, MR_SP,

	MR_DEREF,	/* dereference of a given address */
	MR_LITRL,	/* literal */
	MR_1B,		/* 1 byte */
	MR_2B,		/* 2 bytes */
	MR_4B,		/* 4 bytes */
	MR_8B,		/* 8 bytes */
	MR_ADDR,	/* address */

	MR_MAX
};

enum MoonOpCode {

	/* Memory manipulation */
	MOP_PUT,				 /* named_resource <- resource */
	MOP_PUSH,                /* resource -> stack */
	MOP_POP,                 /* named_resource <- stack */

	/* Data processing */
	MOP_INCR,                /* (register | pointer)++ */
	MOP_DECR,                /* (register | pointer)-- */

	MOP_ADD,                 /* (register | pointer) += (register | pointer) */
	MOP_SUB,                 /* (register | pointer) -= (register | pointer) */
	MOP_MUL,                 /* (register | pointer) *= (register | pointer) */
	MOP_DIV,                 /* (register | pointer) /= (register | pointer) */
	MOP_MOD,                 /* (register | pointer) %= (register | pointer) */

	MOP_AND,                 /* (register | flag) &= (register | flag) */
	MOP_OR,                  /* (register | flag) |= (register | flag) */
	MOP_XOR,                 /* (register | flag) ^= (register | flag) */
	MOP_NOT,                 /* (register | flag) = ~(register | flag) */

	/* Execution control */
	MOP_TEST,				 /* test (register | pointer) (register | pointer) */
	MOP_JUMP,                /* IP = literal */
	MOP_JUMP_EQUAL,          /* if ZF then literal -> IP */
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

struct MoonVm {

	REG_T registers[REG_COUNT];

	FLAG_T zero_flag;
	FLAG_T sign_flag;
	FLAG_T overflow_flag;
	FLAG_T parity_flag;
	FLAG_T carry_flag;

	ADDRESS_T instruction_pointer;
	ADDRESS_T stack_pointer;

	struct {
		CELL_T *data;
		int size, cap;
	} stack;
};

void vm_init(struct MoonVm *vm);
void vm_deinit(struct MoonVm *vm);
void vm_reset(struct MoonVm *vm);
void vm_load(struct MoonVm *vm, CELL_T *data, int size);
void vm_expand_stack(struct MoonVm *vm, int size);

CELL_T vm_next(struct MoonVm *vm);
void vm_next_n(struct MoonVm *vm, CELL_T* dst, int n);

bool vm_next_resource_register(struct MoonVm *vm, CELL_T res, CELL_T **addr, int *size);
bool vm_next_resource_flag(struct MoonVm *vm, CELL_T res, CELL_T **addr, int *size);
bool vm_next_resource_pointer(struct MoonVm *vm, CELL_T res, CELL_T **addr, int *size);
bool vm_next_resource_local(struct MoonVm *vm, CELL_T res, CELL_T **addr, int *size);
bool vm_next_resource_literal(struct MoonVm *vm, CELL_T res, CELL_T **addr, int *size);
bool vm_next_resource_dereference(struct MoonVm *vm, CELL_T res, CELL_T **addr, int *size);
bool vm_next_resource(struct MoonVm *vm, CELL_T res, CELL_T **addr, int *size);
bool vm_next_named_resource(struct MoonVm *vm, CELL_T res, CELL_T **addr, int *size);

bool vm_run(struct MoonVm *vm);
void vm_test(void);

#endif
