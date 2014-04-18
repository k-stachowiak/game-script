#ifndef MOON_ITPR_ALGORITHM_H
#define MOON_ITPR_ALGORITHM_H

#include <string>
#include <vector>

#include "../API/Value.h"
#include "../common/SourceLocation.h"

#include "Stack.h"
#include "Scope.h"

namespace moon {
namespace itpr {

	CValue CallFunction(
		CScope& scope,
		CStack& stack,
		const CSourceLocation& location,
		const std::string& symbol,
		const std::vector<CValue>& argValues);

}
}

#endif