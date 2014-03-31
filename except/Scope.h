#ifndef MOON_EXCEPT_SCOPE_H
#define MOON_EXCEPT_SCOPE_H

#include "../API/Exceptions.h"
#include "../common/SourceLocation.h"

namespace moon {
namespace except {

	struct ExScope {
		struct SymbolAlreadyRegistered : public ExParsingError {
			SymbolAlreadyRegistered(const CSourceLocation& location) :
			ExParsingError{
				location.GetLine(),
				location.GetColumn(),
				"Symbol already registered." }
			{}
		};
		struct SymbolNotRegistered : public ExParsingError {
			SymbolNotRegistered(const CSourceLocation& location) :
			ExCompilationError{
				location.GetLine(),
				location.GetColumn(),
				"Symbol not registered." }
			{}
		};
		struct SymbolIsNotFunction : public ExParsingError {
			SymbolIsNotFunction(const CSourceLocation& location) :
				location.GetLine(),
				location.GetColumn(),
				"Symbol is not a function." }
			{}
		};
		struct FormalActualArgCountMismatch : public ExParsingError {
			FormalActualArgCountMismatch(const CSourceLocation& location) :
			ExParsingError{
				location.GetLine(),
				location.GetColumn(),
				"Formal and actual argument call mismatch." }
			{}
		};
	};

}
}

#endif
