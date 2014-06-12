#include "Scope.h"

#include <stdexcept>
#include <cassert>

#include "Exceptions.h"
#include "Stack.h"
#include "../ast/Bind.h"
#include "../ast/Literal.h"

namespace moon {
namespace itpr {

    void Scope::TryRegisteringBind(
            const Stack& stack,
            const std::string& name,
            const Value& value,
            const SourceLocation& location)
    {
        if (t_binds.find(name) != end(t_binds)) {
            throw ExScopeSymbolAlreadyRegistered{ location, stack };
        }
        t_binds[name] = { value, location };
    }

    std::vector<std::string> Scope::GetAllValues() const
    {
        std::vector<std::string> result;
        for (const auto& pr : t_binds) {
            if (!IsFunction(pr.second.value)) {
                result.push_back(pr.first);
            }
        }
        return result;
    }

    std::vector<std::string> Scope::GetAllFunctions() const
    {
        std::vector<std::string> result;
        for (const auto& pr : t_binds) {
            if (IsFunction(pr.second.value)) {
                result.push_back(pr.first);
            }
        }
        return result;
    }

    Value Scope::GetValue(
        const std::string& name,
        const SourceLocation& location,
        const Stack& stack)
    {
        return GetValueStore(name, location, stack).value;
    } 

    const ValueStore GlobalScope::GetValueStore(
        const std::string& name,
        const SourceLocation& location,
        const Stack& stack) const
    {
        if (t_binds.find(name) == end(t_binds)) {
            throw ExScopeSymbolNotRegistered{ location, stack };
        } else {
            return t_binds.at(name);
        }
    }

    const ValueStore LocalScope::GetValueStore(
        const std::string& name,
        const SourceLocation& location,
        const Stack& stack) const
    {
        if (t_binds.find(name) == end(t_binds)) {
            return m_globalScope.GetValueStore(name, location, stack);
        }
        else {
            return t_binds.at(name);
        }
    }

    std::map<std::string, Capture>
    LocalScope::CaptureNonGlobals(const std::vector<std::string>& names) const
    {
        std::map<std::string, Capture> result;
        for(const std::string& name : names) {
            auto found = t_binds.find(name);
            if (found == end(t_binds)) {
                continue;
            } else {
                result[name] = {
                    found->second.value,
                    found->second.location
                };
            }
        }
        return result;
    }

}
}
