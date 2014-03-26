#ifndef MOON_EXCEPT_SCOPE_H
#define MOON_EXCEPT_SCOPE_H

#include "CompilationError.h"

namespace moon {
namespace except {

	struct ExScope {
		struct SymbolAlreadyRegistered : public ExCompilationError {
			SymbolAlreadyRegistered(int line, int column) :
				ExCompilationError{ line, column, "Symbol already registered." }
			{}
		};
		struct SymbolNotRegistered : public ExCompilationError {
			SymbolNotRegistered(int line, int column) :
				ExCompilationError{ line, column, "Symbol not registered." }
			{}
		};
		struct SymbolIsNotFunction : public ExCompilationError {
			SymbolIsNotFunction(int line, int column) :
				ExCompilationError{ line, column, "Symbol is not a function." }
			{}
		};
		struct FormalActualArgCountMismatch : public ExCompilationError {
			FormalActualArgCountMismatch(int line, int column) :
				ExCompilationError{ line, column, "Formal and actual argument call mismatch." }
			{}
		};
	};

}
}

#endif
