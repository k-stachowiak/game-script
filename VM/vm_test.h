/* Copytight (C) 2015 Krzysztof Stachowiak */

#include "vm.h"
#include "test.h"

#define LEN(x) (sizeof(x) / sizeof(*x))

#define ac 0xac
#define ca 0xca
#define deadcode 0xdeadc0de
#define decoadde 0xdec0adde
#define deededde 0xdeededde

void vm_test_program(
		struct TestContext *tc,
		struct MoonVm *vm,
		CELL_T *program,
		int program_size,
		void(*preconditions)(struct MoonVm*),
		void(*postconditions)(struct TestContext *, struct MoonVm*));

void vm_test_put(struct TestContext *tc);
void vm_test_push_pop(struct TestContext *tc);
void vm_test_incr_decr(struct TestContext *tc);
void vm_test_arythmetic(struct TestContext *tc);
void vm_test(void);