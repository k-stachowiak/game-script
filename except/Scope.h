#ifndef MOON_EXCEPT_SCOPE_H
#define MOON_EXCEPT_SCOPE_H

#include <stdexcept>

namespace moon {
namespace except {

	struct ExScope {
		struct SymbolAlreadyRegistered : public std::runtime_error {
			SymbolAlreadyRegistered() :
				std::runtime_error{ "Symbol already registered." }
			{}
		};
		struct SymbolNotRegistered : public std::runtime_error {
			SymbolNotRegistered() :
				std::runtime_error{ "Symbol not registered." }
			{}
		};
		struct SymbolIsNotFunction : public std::runtime_error {
			SymbolIsNotFunction() :
				std::runtime_error{ "Symbol is not a function." }
			{}
		};
		struct FormalActualArgCountMismatch : public std::runtime_error {
			FormalActualArgCountMismatch() :
				std::runtime_error{ "Formal and actual argument call mismatch." }
			{}
		};
	};

}
}

#endif