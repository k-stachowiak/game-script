#ifndef MOON_EXCEPT_SEXPR_H
#define MOON_EXCEPT_SEXPR_H

#include <stdexcept>

namespace moon {
namespace except {

	struct ExTokenizer {
		struct NonDelimitedStringOrCharacter : public std::runtime_error {
			NonDelimitedStringOrCharacter() :
				std::runtime_error{ "Non-delimited string or character." }
			{}
		};		
	};

}
}

#endif