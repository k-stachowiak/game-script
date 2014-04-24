#ifndef MOON_ITPR_VALUE_H
#define MOON_ITPR_VALUE_H

#include <string>
#include <vector>
#include <map>

#include "../common/SourceLocation.h"

namespace moon {

	namespace ast {
		class CAstFunction;
	}

	namespace itpr {
		class CScope;
	}

	class CValue;
	struct SCapture;

	enum class EValueType {
		INTEGER,
		REAL,
		CHARACTER,
		STRING,
		BOOLEAN,
		COMPOUND,
		FUNCTION
	};

	enum class ECompoundType {
		ARRAY,
		TUPLE
	};

	struct SCompoundValue {
		ECompoundType type = ECompoundType::ARRAY;
		std::vector<CValue> values;
		SCompoundValue() = default;
		SCompoundValue(ECompoundType new_type, std::vector<CValue> new_values);
	};

	struct SFunctionValue {
		const ast::CAstFunction* definition = nullptr;
		std::map<std::string, SCapture> captures;
		std::vector<CValue> appliedArgs;
		SFunctionValue() = default;
		SFunctionValue(
			const ast::CAstFunction* new_definition,
			std::map<std::string, SCapture> new_captures,
			std::vector<CValue> new_appliedArgs);
	};

	class CValue {

		EValueType m_type = EValueType::INTEGER;

		long m_integer = -1;
		double m_real = -1;
		char m_character = -1;
		std::string m_string;
		int m_boolean = -1;
		SCompoundValue m_compound;
		SFunctionValue m_function;

		CValue(
			EValueType type,
			long integer,
			double real,
			char character,
			std::string string,
			int boolean,
			ECompoundType compoundType,
			std::vector<CValue> compoundValues,
			const ast::CAstFunction* funcDefinition,
			std::map<std::string, SCapture> funcCaptures,
			std::vector<CValue> funcAppliedArgs);

	public:
		static CValue MakeInteger(long value);
		static CValue MakeReal(double value);
		static CValue MakeCharacter(char value);
		static CValue MakeString(std::string value);
		static CValue MakeBoolean(int value);
		static CValue MakeCompound(ECompoundType type, const std::vector<CValue>& values);
		static CValue MakeFunction(
			const ast::CAstFunction* definition,
			const std::map<std::string, SCapture>& captures,
			const std::vector<CValue>& appliedArgs);

		friend bool IsCompound(const CValue& value) { return value.m_type == EValueType::COMPOUND; }		
		friend bool IsAtomic(const CValue& value) { return value.m_type != EValueType::COMPOUND; }

		friend bool IsArray(const CValue& value) { return IsCompound(value) && value.GetCompoundType() == ECompoundType::ARRAY; }		
		friend bool IsTuple(const CValue& value) { return IsCompound(value) && value.GetCompoundType() == ECompoundType::TUPLE; }
		friend bool IsTuple(const CValue& value, int size)
		{
			return IsCompound(value) && 
				   value.GetCompoundType() == ECompoundType::TUPLE &&
				   value.m_compound.values.size() == size;
		}

		friend bool IsFunction(const CValue& value) { return value.m_type == EValueType::FUNCTION; }
		friend bool IsInteger(const CValue& value) { return value.GetType() == EValueType::INTEGER; }
		friend bool IsReal(const CValue& value) { return value.GetType() == EValueType::REAL; }
		friend bool IsString(const CValue& value) { return value.GetType() == EValueType::STRING; }

		static bool TypesEqual(const CValue& lhs, const CValue& rhs);

		CValue() = default;
		CValue& operator=(const CValue&) = default;

		EValueType GetType() const { return m_type; }
		long GetInteger() const { return m_integer; }
		double GetReal() const { return m_real; }
		char GetCharacter() const { return m_character; }
		const std::string& GetString() const { return m_string; }
		int GetBoolean() const { return m_boolean; }

		ECompoundType GetCompoundType() const { return m_compound.type; }
		const std::vector<CValue>& GetCompound() const { return m_compound.values; }
		
		unsigned GetFuncArity() const;
		const ast::CAstFunction& GetFuncDef() const { return *(m_function.definition); }
		const std::vector<CValue>& GetAppliedArgs() const { return m_function.appliedArgs; }
		const std::map<std::string, SCapture>& GetFuncCaptures() const;
	};

	struct SCapture {
		CValue value;
		CSourceLocation location;
		SCapture() = default;
		SCapture(CValue new_value, CSourceLocation new_location);
	};

}

#endif
