#ifndef MOON_EXCEPT_AST_H
#define MOON_EXCEPT_AST_H

#include "CompilationError.h"

namespace moon {
namespace except {

	struct ExAst {
		struct ReferenceToFunctionEvaluated : public ExCompilationError {
			ReferenceToFunctionEvaluated(const CSourceLocation& location) :
				ExCompilationError{ location, "Reference to function evaluated." }
			{}
		};
		struct ArithmeticTypeMismatch : public ExCompilationError {
			ArithmeticTypeMismatch(const CSourceLocation& location) :
				ExCompilationError{ location, "Arithmetic type mismatch." }
			{}
		};
		struct BindParsingFailed : public ExCompilationError {
			BindParsingFailed(const CSourceLocation& location) :
				ExCompilationError{ location, "Bind parsing failed." }
			{}
		};
	};

}
}

#endif
