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

	bool CValue::TypesEqual(const CValue& lhs, const CValue& rhs)
	{
		if (IsAtomic(lhs) && IsAtomic(rhs)) {
			return lhs.GetType() == rhs.GetType();

		}
		else if (IsCompound(lhs) && IsCompound(rhs)) {

			// Compare compound types and sizes.
			if (lhs.m_compoundType != rhs.m_compoundType ||
				lhs.m_compoundValues.size() != rhs.m_compoundValues.size()) {
				return false;
			}

			// Compare child-wise.
			unsigned commonSize = lhs.m_compoundValues.size();
			for (unsigned i = 0; i < commonSize; ++i) {
				if (!TypesEqual(lhs.m_compoundValues[i], rhs.m_compoundValues[i])) {
					return false;
				}
			}

			return true;

		}
		else {
			return false;
		}
	}

}
