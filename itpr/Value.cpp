#include <algorithm>
#include <iterator>

#include "../ast/FuncDef.h"

#include "Value.h"

namespace moon {
namespace itpr {

    CompoundValue::CompoundValue(
            CompoundType new_type,
            std::vector<Value> new_values) :
        type{ new_type },
        values{ new_values }
    {}

    FunctionValue::FunctionValue(
            const ast::AstFunction* new_definition,
            std::map<std::string, Capture> new_captures,
            std::vector<Value> new_appliedArgs) :
        definition{ new_definition },
        captures{ new_captures },
        appliedArgs{ new_appliedArgs }
    {}

    Value::Value(
        ValueType type,
        long integer,
        double real,
        char character,
        std::string string,
        int boolean,
        CompoundType compoundType,
        std::vector<Value> compoundValues,
        const ast::AstFunction* funcDefinition,
        std::map<std::string, Capture> funcCaptures,
        std::vector<Value> funcAppliedArgs) :
        m_type{ type },
        m_integer{ integer },
        m_real{ real },
        m_character{ character },
        m_string{ string },
        m_boolean{ boolean },
        m_compound{ compoundType, compoundValues },
        m_function{ funcDefinition, funcCaptures, funcAppliedArgs }
    {}

    Value Value::MakeInteger(long value)
    {
        Value result{
            ValueType::INTEGER, value,
            0, 0, {}, 0,
            CompoundType::ARRAY, {},
            nullptr, {}, {}
        };
        return result;
    }

    Value Value::MakeReal(double value)
    {
        Value result{
            ValueType::REAL,
            0,
            value,
            0, {}, 0,
            CompoundType::ARRAY, {},
            nullptr, {}, {}
        };
        return result;
    }

    Value Value::MakeCharacter(char value)
    {
        Value result{
            ValueType::CHARACTER,
            0, 0,
            value,
            {}, 0,
            CompoundType::ARRAY, {},
            nullptr, {}, {}
        };
        return result;
    }

    Value Value::MakeString(std::string value)
    {
        Value result{
            ValueType::STRING,
            0, 0, 0,
            value,
            0,
            CompoundType::ARRAY, {},
            nullptr, {}, {}
        };
        return result;
    }

    Value Value::MakeBoolean(int value)
    {
        Value result{
            ValueType::BOOLEAN,
            0, 0, 0, {},
            value,
            CompoundType::ARRAY, {},
            nullptr, {}, {}
        };
        return result;
    }

    Value Value::MakeCompound(CompoundType type, const std::vector<Value>& values)
    {
        Value result{
            ValueType::COMPOUND,
            0, 0, 0, {}, 0,
            type, values,
            nullptr, {}, {}
        };

        return result;
    }
    
    Value Value::MakeFunction(
        const ast::AstFunction* definition,
        const std::map<std::string, Capture>& captures,
        const std::vector<Value>& appliedArgs)
    {
        Value result{
            ValueType::FUNCTION,
            0, 0, 0, {}, 0,
            CompoundType::ARRAY, {},
            definition, captures, appliedArgs
        };

        return result;
    }
    
    bool Value::TypesEqual(const Value& lhs, const Value& rhs)
    {
        if (IsAtomic(lhs) && IsAtomic(rhs)) {
            return lhs.GetType() == rhs.GetType();

        } else if (IsCompound(lhs) && IsCompound(rhs)) {

            // Compare compound types and sizes.
            if (lhs.m_compound.type != rhs.m_compound.type ||
                lhs.m_compound.values.size() != rhs.m_compound.values.size()) {
                return false;
            }

            // Compare child-wise.
            unsigned commonSize = lhs.m_compound.values.size();
            for (unsigned i = 0; i < commonSize; ++i) {
                if (!TypesEqual(lhs.m_compound.values[i], rhs.m_compound.values[i])) {
                    return false;
                }
            }

            return true;

        } else if (IsFunction(lhs) && IsFunction(rhs)) {
            return lhs.GetFuncArity() == rhs.GetFuncArity();

        } else {
            return false;
        }
    }

    unsigned Value::GetFuncArity() const
    {
        return m_function.definition->GetArgsCount() - m_function.appliedArgs.size();
    }

    const std::map<std::string, Capture>& Value::GetFuncCaptures() const
    {
        return m_function.captures;
    }

    Capture::Capture(
            Value new_value,
            SourceLocation new_location) :
        value(new_value),
        location(new_location)
    {}

}
}
