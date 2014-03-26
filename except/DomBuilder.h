#ifndef MOON_EXCEPT_DOM_BUILDER_H
#define MOON_EXCEPT_DOM_BUILDER_H

#include "CompilationError.h"

namespace moon {
namespace except {

	struct ExDomBuilder {
		struct UnexpectedCompoundDelimiter : public ExCompilationError {
			UnexpectedCompoundDelimiter(int line, int column) :
				ExCompilationError{ line, column, "Unexpected compound delimiter." }
			{}
		};
		struct UnclosedCompoundNode : public ExCompilationError {
			UnclosedCompoundNode(int line, int column) :
				ExCompilationError{ line, column, "Unclosed component node." }
			{}
		};
	};

}
}

#endif
