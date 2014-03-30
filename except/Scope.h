#ifndef MOON_EXCEPT_SCOPE_H
#define MOON_EXCEPT_SCOPE_H

#include "../API/Exceptions.h"
#include "../API/SourceLocation.h"

namespace moon {
namespace except {

	struct ExScope {
		struct SymbolAlreadyRegistered : public ExCompilationError {
			SymbolAlreadyRegistered(const CSourceLocation& location) :
			ExCompilationError{
				location.GetLine(),
				location.GetColumn(), 
				"Symbol already registered." }
			{}
		};
		struct SymbolNotRegistered : public ExCompilationError {
			SymbolNotRegistered(const CSourceLocation& location) :
			ExCompilationError{
				location.GetLine(),
				location.GetColumn(),
				"Symbol not registered." }
			{}
		};
		struct SymbolIsNotFunction : public ExCompilationError {
			SymbolIsNotFunction(const CSourceLocation& location) :
			ExCompilationError{
				location.GetLine(),
				location.GetColumn(), 
				"Symbol is not a function." }
			{}
		};
		struct FormalActualArgCountMismatch : public ExCompilationError {
			FormalActualArgCountMismatch(const CSourceLocation& location) :
			ExCompilationError{
				location.GetLine(),
				location.GetColumn(), 
				"Formal and actual argument call mismatch." }
			{}
		};
	};

}
}

#endif
