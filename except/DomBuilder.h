#ifndef MOON_EXCEPT_DOM_BUILDER_H
#define MOON_EXCEPT_DOM_BUILDER_H

#include "../API/SourceLocation.h"
#include "../API/Exceptions.h"

namespace moon {
namespace except {

	struct ExDomBuilder {
		struct UnexpectedCompoundDelimiter : public ExCompilationError {
			UnexpectedCompoundDelimiter(const CSourceLocation& location) :
			ExCompilationError{
				location.GetLine(),
				location.GetColumn(), 
				"Unexpected compound delimiter." }
			{}
		};
		struct UnclosedCompoundNode : public ExCompilationError {
			UnclosedCompoundNode(const CSourceLocation& location) :
			ExCompilationError{
				location.GetLine(),
				location.GetColumn(), 
				"Unclosed component node." }
			{}
		};
	};

}
}

#endif
