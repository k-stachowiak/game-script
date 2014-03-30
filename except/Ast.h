#ifndef MOON_EXCEPT_AST_H
#define MOON_EXCEPT_AST_H

#include "../API/Exceptions.h"
#include "../API/SourceLocation.h"

namespace moon {
namespace except {

	struct ExAst {
		struct ReferenceToFunctionEvaluated : public ExCompilationError {
			ReferenceToFunctionEvaluated(const CSourceLocation& location) :
			ExCompilationError{
				location.GetLine(),
				location.GetColumn(),
				"Reference to function evaluated." }
			{}
		};
		struct ArithmeticTypeMismatch : public ExCompilationError {
			ArithmeticTypeMismatch(const CSourceLocation& location) :
			ExCompilationError{
				location.GetLine(),
				location.GetColumn(), 
				"Arithmetic type mismatch." }
			{}
		};
		struct BindParsingFailed : public ExCompilationError {
			BindParsingFailed(const CSourceLocation& location) :
			ExCompilationError{
				location.GetLine(),
				location.GetColumn(), 
				"Bind parsing failed." }
			{}
		};
	};

}
}

#endif
