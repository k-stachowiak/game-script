#ifndef MOON_EXCEPT_SEXPR_H
#define MOON_EXCEPT_SEXPR_H

#include "../API/SourceLocation.h"
#include "CompilationError.h"

namespace moon {
namespace except {

	struct ExTokenizer {
		struct NonDelimitedStringOrCharacter : public ExCompilationError {
			NonDelimitedStringOrCharacter(const CSourceLocation& location) :
				ExCompilationError{ location, "Non-delimited string or character." }
			{}
		};
	};

}
}

#endif
