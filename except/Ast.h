#ifndef MOON_EXCEPT_AST_H
#define MOON_EXCEPT_AST_H

#include "../common/SourceLocation.h"
#include "../API/Exceptions.h"

namespace moon {
namespace except {

	struct ExAst {
		struct ReferenceToFunctionEvaluated : public ExParsingError {
			ReferenceToFunctionEvaluated(const CSourceLocation& location) :
				ExParsingError{
					location.GetLine(),
					location.GetColumn(),
					"Reference to function evaluated." }
			{}
		};
		struct ArithmeticTypeMismatch : public ExParsingError {
			ArithmeticTypeMismatch(const CSourceLocation& location) :
				ExParsingError{
					location.GetLine(),
					location.GetColumn(),
					"Arithmetic type mismatch." }
			{}
		};
		struct BindParsingFailed : public ExParsingError {
			BindParsingFailed(const CSourceLocation& location) :
				ExParsingError{
					location.GetLine(),
					location.GetColumn(),
					"Bind parsing failed." }
			{}
		};
	};

}
}

#endif
