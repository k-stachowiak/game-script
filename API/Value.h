#ifndef MOON_ITPR_VALUE_H
#define MOON_ITPR_VALUE_H

#include <string>
#include <vector>

namespace moon {

	enum class EValueType {
		INTEGER,
		REAL,
		CHARACTER,
		STRING,
		BOOLEAN,
		COMPOUND
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

		CValue(
			EValueType type,
			long integer,
			double real,
			char character,
			std::string string,
			int boolean,
			ECompoundType compoundType,
			std::vector<CValue> compoundValues);

	public:
		static CValue MakeInteger(long value);
		static CValue MakeReal(double value);
		static CValue MakeCharacter(char value);
		static CValue MakeString(std::string value);
		static CValue MakeBoolean(int value);
		static CValue MakeCompound(ECompoundType type, std::vector<CValue> values);

		friend bool IsInteger(const CValue& value)
		{
			return value.GetType() == EValueType::INTEGER;
		}

		friend bool IsReal(const CValue& value)
		{
			return value.GetType() == EValueType::REAL;
		}

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
	};

}

#endif
