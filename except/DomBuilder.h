#ifndef MOON_EXCEPT_DOM_BUILDER_H
#define MOON_EXCEPT_DOM_BUILDER_H

#include <stdexcept>

namespace moon {
namespace except {

	struct ExDomBuilder {
		struct UnexpectedCompoundDelimiter : public std::runtime_error {
			UnexpectedCompoundDelimiter() :
				std::runtime_error{ "Unexpected compound delimiter." }
			{}
		};
		struct UnclosedCompoundNode : public std::runtime_error {
			UnclosedCompoundNode() :
				std::runtime_error{ "Unclosed component node." }
			{}
		};
	};

}
}

#endif