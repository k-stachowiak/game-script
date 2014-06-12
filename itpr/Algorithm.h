#ifndef MOON_ITPR_ALGORITHM_H
#define MOON_ITPR_ALGORITHM_H

#include <string>
#include <vector>

#include "Value.h"
#include "../common/SourceLocation.h"

#include "Stack.h"
#include "Scope.h"

namespace moon {
namespace itpr {

    Value CallFunction(
        Scope& scope,
        Stack& stack,
        const SourceLocation& location,
        const std::string& symbol,
        const std::vector<Value>& argValues);

}
}

#endif
