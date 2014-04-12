#ifndef MOON_ITPR_VALUE_H
#define MOON_ITPR_VALUE_H

#include <string>
#include <vector>

namespace moon {

	// WOW, I've got what I wanted - higher order functions :]
	namespace itpr {
		class CAstFunction;
		class CScope;
	}

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

	class CValue {
		EValueType m_type = EValueType::INTEGER;

		long m_integer = -1;
		double m_real = -1;
		char m_character = -1;
		std::string m_string;
		int m_boolean = -1;

		ECompoundType m_compoundType = ECompoundType::ARRAY;
		std::vector<CValue> m_compoundValues;

		const itpr::CAstFunction* m_funcDef;
		std::vector<CValue> m_appliedArgs;

		CValue(
			EValueType type,
			long integer,
			double real,
			char character,
			std::string string,
			int boolean,
			ECompoundType compoundType,
			std::vector<CValue> compoundValues,
			const itpr::CAstFunction* funcDef,
			std::vector<CValue> appliedArgs);

	public:
		static CValue MakeInteger(long value);
		static CValue MakeReal(double value);
		static CValue MakeCharacter(char value);
		static CValue MakeString(std::string value);
		static CValue MakeBoolean(int value);
		static CValue MakeCompound(ECompoundType type, std::vector<CValue> values);
		static CValue MakeFunction(const itpr::CAstFunction* funcDef, std::vector<CValue> appliedArgs);

		friend bool IsCompound(const CValue& value)
		{
			return value.m_type == EValueType::COMPOUND;
		}

		friend bool IsAtomic(const CValue& value)
		{
			return value.m_type != EValueType::COMPOUND;
		}

		friend bool IsFunction(const CValue& value)
		{
			return value.m_type == EValueType::FUNCTION;
		}

		friend bool IsInteger(const CValue& value)
		{
			return value.GetType() == EValueType::INTEGER;
		}

		friend bool IsReal(const CValue& value)
		{
			return value.GetType() == EValueType::REAL;
		}

		static bool TypesEqual(const CValue& lhs, const CValue& rhs);

		CValue() = default;
		CValue& operator=(const CValue&) = default;

		EValueType GetType() const { return m_type; }
		long GetInteger() const { return m_integer; }
		double GetReal() const { return m_real; }
		char GetCharacter() const { return m_character; }
		const std::string& GetString() const { return m_string; }
		int GetBoolean() const { return m_boolean; }

		ECompoundType GetCompoundType() const { return m_compoundType; }
		const std::vector<CValue>& GetCompound() const { return m_compoundValues; }
		
		int CValue::GetFuncArity() const;
		const itpr::CAstFunction* GetFuncDef() const { return m_funcDef; }
		std::vector<CValue>& GetAppliedArgs() { return m_appliedArgs; }
	};

}

#endif
