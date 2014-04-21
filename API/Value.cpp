#include <algorithm>
#include <iterator>

#include "../ast/FuncDef.h"

#include "Value.h"

namespace moon {

	SCompoundValue::SCompoundValue(
			ECompoundType new_type,
			std::vector<CValue> new_values) :
		type{ new_type },
		values{ new_values }
	{}

	SFunctionValue::SFunctionValue(
			const ast::CAstFunction* new_definition,
			std::map<std::string, SCapture> new_captures,
			std::vector<CValue> new_appliedArgs) :
		definition{ new_definition },
		captures{ new_captures },
		appliedArgs{ new_appliedArgs }
	{}

	CValue::CValue(
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
		std::vector<CValue> funcAppliedArgs) :
		m_type{ type },
		m_integer{ integer },
		m_real{ real },
		m_character{ character },
		m_string{ string },
		m_boolean{ boolean },
		m_compound{ compoundType, compoundValues },
		m_function{ funcDefinition, funcCaptures, funcAppliedArgs }
	{}

	CValue CValue::MakeInteger(long value)
	{
		CValue result{
			EValueType::INTEGER, value,
			0, 0, {}, 0,
			ECompoundType::ARRAY, {},
			nullptr, {}, {}
		};
		return result;
	}

	CValue CValue::MakeReal(double value)
	{
		CValue result{
			EValueType::REAL,
			0,
			value,
			0, {}, 0,
			ECompoundType::ARRAY, {},
			nullptr, {}, {}
		};
		return result;
	}

	CValue CValue::MakeCharacter(char value)
	{
		CValue result{
			EValueType::CHARACTER,
			0, 0,
			value,
			{}, 0,
			ECompoundType::ARRAY, {},
			nullptr, {}, {}
		};
		return result;
	}

	CValue CValue::MakeString(std::string value)
	{
		CValue result{
			EValueType::STRING,
			0, 0, 0,
			value,
			0,
			ECompoundType::ARRAY, {},
			nullptr, {}, {}
		};
		return result;
	}

	CValue CValue::MakeBoolean(int value)
	{
		CValue result{
			EValueType::BOOLEAN,
			0, 0, 0, {},
			value,
			ECompoundType::ARRAY, {},
			nullptr, {}, {}
		};
		return result;
	}

	CValue CValue::MakeCompound(ECompoundType type, const std::vector<CValue>& values)
	{
		CValue result{
			EValueType::COMPOUND,
			0, 0, 0, {}, 0,
			type, values,
			nullptr, {}, {}
		};

		return result;
	}
	
	CValue CValue::MakeFunction(
		const ast::CAstFunction* definition,
		const std::map<std::string, SCapture>& captures,
		const std::vector<CValue>& appliedArgs)
	{
		CValue result{
			EValueType::FUNCTION,
			0, 0, 0, {}, 0,
			ECompoundType::ARRAY, {},
			definition, captures, appliedArgs
		};

		return result;
	}
	
	bool CValue::TypesEqual(const CValue& lhs, const CValue& rhs)
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

	unsigned CValue::GetFuncArity() const
	{
		return m_function.definition->GetArgsCount() - m_function.appliedArgs.size();
	}

	const std::map<std::string, SCapture>& CValue::GetFuncCaptures() const
	{
		return m_function.captures;
	}

	SCapture::SCapture(
			CValue new_value,
			CSourceLocation new_location) :
		value(new_value),
		location(new_location)
	{}

}
