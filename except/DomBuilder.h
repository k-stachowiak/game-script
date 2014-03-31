#ifndef MOON_EXCEPT_DOM_BUILDER_H
#define MOON_EXCEPT_DOM_BUILDER_H

#include "../common/SourceLocation.h"
#include "../API/Exceptions.h"

namespace moon {
namespace except {

	struct ExDomBuilder {
		struct UnexpectedCompoundDelimiter : public ExParsingError {
			UnexpectedCompoundDelimiter(const CSourceLocation& location) :
			ExParsingError{
				location.GetLine(),
				location.GetColumn(),
				"Unexpected compound delimiter." }
			{}
		};
		struct UnclosedCompoundNode : public ExParsingError {
			UnclosedCompoundNode(const CSourceLocation& location) :
			ExParsingError{
				location.GetLine(),
				location.GetColumn(),
				"Unclosed component node." }
			{}
		};
	};

}
}

#endif
