#include "Value.h"

namespace moon {

	CValue::CValue(
		EValueType type,
		long integer,
		double real,
		char character,
		std::string string,
		int boolean,
		ECompoundType compoundType,
		std::vector<CValue> compoundValues) :
		m_type{ type },
		m_integer{ integer },
		m_real{ real },
		m_character{ character },
		m_string{ string },
		m_boolean{ boolean },
		m_compoundType{ compoundType },
		m_compoundValues{ compoundValues }
	{}

	CValue CValue::MakeInteger(long value)
	{
		CValue result{
			EValueType::INTEGER, value,
			0, 0, {}, 0, ECompoundType::ARRAY, {}
		};
		return result;
	}

	CValue CValue::MakeReal(double value)
	{
		CValue result{
			EValueType::REAL,
			0,
			value,
			0, {}, 0, ECompoundType::ARRAY, {}
		};
		return result;
	}

	CValue CValue::MakeCharacter(char value)
	{
		CValue result{
			EValueType::CHARACTER,
			0, 0,
			value,
			{}, 0, ECompoundType::ARRAY, {}
		};
		return result;
	}

	CValue CValue::MakeString(std::string value)
	{
		CValue result{
			EValueType::STRING,
			0, 0, 0,
			value,
			0, ECompoundType::ARRAY, {}
		};
		return result;
	}

	CValue CValue::MakeBoolean(int value)
	{
		CValue result{
			EValueType::BOOLEAN,
			0, 0, 0, {},
			value,
			ECompoundType::ARRAY, {}
		};
		return result;
	}

	CValue CValue::MakeCompound(ECompoundType type, std::vector<CValue> values)
	{
		CValue result{
			EValueType::COMPOUND,
			0, 0, 0, {}, 0,
			type, values
		};
		return result;
	}

}
