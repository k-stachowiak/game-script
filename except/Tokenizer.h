#ifndef MOON_EXCEPT_SEXPR_H
#define MOON_EXCEPT_SEXPR_H

#include "../common/SourceLocation.h"
#include "../API/Exceptions.h"

namespace moon {
namespace except {

	struct ExTokenizer {
		struct NonDelimitedStringOrCharacter : public ExParsingError {
			NonDelimitedStringOrCharacter(const CSourceLocation& location) :
				ExParsingError{
					location.GetLine(),
					location.GetColumn(),
					"Non-delimited string or character." }
			{}
		};
	};

}
}

#endif
