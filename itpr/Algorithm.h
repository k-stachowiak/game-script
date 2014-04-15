#ifndef MOON_ITPR_ALGORITHM_H
#define MOON_ITPR_ALGORITHM_H

#include <memory>
#include <string>
#include <vector>

namespace moon {

	class CValue;
	class CSourceLocation;

namespace itpr {

	class CScope;
	class CStack;

	CValue CallFunction(
			const std::string& symbol,
			const std::vector<CValue>& argValues,
			const CSourceLocation& location,
			std::shared_ptr<CScope> scope,
			CStack& stack);

}
}

#endif
