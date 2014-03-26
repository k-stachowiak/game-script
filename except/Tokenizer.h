#ifndef MOON_EXCEPT_SEXPR_H
#define MOON_EXCEPT_SEXPR_H

#include "CompilationError.h"

namespace moon {
namespace except {

	struct ExTokenizer {
		struct NonDelimitedStringOrCharacter : public ExCompilationError {
			NonDelimitedStringOrCharacter(int line, int column) :
				ExCompilationError{ line, column, "Non-delimited string or character." }
			{}
		};
	};

}
}

#endif
