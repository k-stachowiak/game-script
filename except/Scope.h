#ifndef MOON_EXCEPT_SCOPE_H
#define MOON_EXCEPT_SCOPE_H

#include "CompilationError.h"

namespace moon {
namespace except {

	struct ExScope {
		struct SymbolAlreadyRegistered : public ExCompilationError {
			SymbolAlreadyRegistered(const CSourceLocation& location) :
			ExCompilationError{ location, "Symbol already registered." }
			{}
		};
		struct SymbolNotRegistered : public ExCompilationError {
			SymbolNotRegistered(const CSourceLocation& location) :
			ExCompilationError{ location, "Symbol not registered." }
			{}
		};
		struct SymbolIsNotFunction : public ExCompilationError {
			SymbolIsNotFunction(const CSourceLocation& location) :
			ExCompilationError{ location, "Symbol is not a function." }
			{}
		};
		struct FormalActualArgCountMismatch : public ExCompilationError {
			FormalActualArgCountMismatch(const CSourceLocation& location) :
			ExCompilationError{ location, "Formal and actual argument call mismatch." }
			{}
		};
	};

}
}

#endif
