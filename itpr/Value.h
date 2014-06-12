#ifndef MOON_ITPR_VALUE_H
#define MOON_ITPR_VALUE_H

#include <string>
#include <vector>
#include <map>

#include "../common/SourceLocation.h"

namespace moon {
    
    namespace ast {
        class AstFunction;
    }

namespace itpr {

    class Scope;
    class Value;
    struct Capture;

    enum class ValueType {
        INTEGER,
        REAL,
        CHARACTER,
        STRING,
        BOOLEAN,
        COMPOUND,
        FUNCTION
    };

    enum class CompoundType {
        ARRAY,
        TUPLE
    };

    struct CompoundValue {
        CompoundType type = CompoundType::ARRAY;
        std::vector<Value> values;
        CompoundValue() = default;
        CompoundValue(CompoundType new_type, std::vector<Value> new_values);
    };

    struct FunctionValue {
        const ast::AstFunction* definition = nullptr;
        std::map<std::string, Capture> captures;
        std::vector<Value> appliedArgs;
        FunctionValue() = default;
        FunctionValue(
            const ast::AstFunction* new_definition,
            std::map<std::string, Capture> new_captures,
            std::vector<Value> new_appliedArgs);
    };

    class Value {

        ValueType m_type = ValueType::INTEGER;

        long m_integer = -1;
        double m_real = -1;
        char m_character = -1;
        std::string m_string;
        int m_boolean = -1;
        CompoundValue m_compound;
        FunctionValue m_function;

        Value(
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
            std::vector<Value> funcAppliedArgs);

    public:
        static Value MakeInteger(long value);
        static Value MakeReal(double value);
        static Value MakeCharacter(char value);
        static Value MakeString(std::string value);
        static Value MakeBoolean(int value);
        static Value MakeCompound(CompoundType type, const std::vector<Value>& values);
        static Value MakeFunction(
            const ast::AstFunction* definition,
            const std::map<std::string, Capture>& captures,
            const std::vector<Value>& appliedArgs);

        friend bool IsCompound(const Value& value) { return value.m_type == ValueType::COMPOUND; }
        friend bool IsAtomic(const Value& value) { return value.m_type != ValueType::COMPOUND; }

        friend bool IsArray(const Value& value) { return IsCompound(value) && value.GetCompoundType() == CompoundType::ARRAY; }        
        friend bool IsTuple(const Value& value) { return IsCompound(value) && value.GetCompoundType() == CompoundType::TUPLE; }
        friend bool IsTuple(const Value& value, unsigned size)
        {
            return IsCompound(value) && 
                   value.GetCompoundType() == CompoundType::TUPLE &&
                   value.m_compound.values.size() == size;
        }

        friend bool IsFunction(const Value& value) { return value.m_type == ValueType::FUNCTION; }
        friend bool IsInteger(const Value& value) { return value.GetType() == ValueType::INTEGER; }
        friend bool IsReal(const Value& value) { return value.GetType() == ValueType::REAL; }
        friend bool IsString(const Value& value) { return value.GetType() == ValueType::STRING; }

        static bool TypesEqual(const Value& lhs, const Value& rhs);

        Value() = default;
        Value& operator=(const Value&) = default;

        ValueType GetType() const { return m_type; }
        long GetInteger() const { return m_integer; }
        double GetReal() const { return m_real; }
        char GetCharacter() const { return m_character; }
        const std::string& GetString() const { return m_string; }
        int GetBoolean() const { return m_boolean; }

        CompoundType GetCompoundType() const { return m_compound.type; }
        const std::vector<Value>& GetCompound() const { return m_compound.values; }
        
        unsigned GetFuncArity() const;
        const ast::AstFunction& GetFuncDef() const { return *(m_function.definition); }
        const std::vector<Value>& GetAppliedArgs() const { return m_function.appliedArgs; }
        const std::map<std::string, Capture>& GetFuncCaptures() const;
    };

    struct Capture {
        Value value;
        SourceLocation location;
        Capture() = default;
        Capture(Value new_value, SourceLocation new_location);
    };

}
}

#endif
