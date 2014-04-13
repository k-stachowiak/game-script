#include "AstCompound.h"
#include "Exceptions.h"

namespace moon {
namespace itpr {

	CAstCompound::CAstCompound(
		const CSourceLocation& location,
		ECompoundType type, 
		std::vector<std::unique_ptr<CAstNode>>&& expressions) :
		CAstNode{ location },
		m_type{ type },
		m_expressions{ std::move(expressions) }
	{}

	CValue CAstCompound::Evaluate(std::shared_ptr<CScope> scope, CStack& stack) const
	{
		std::vector<CValue> values;
		for (const auto& expression : m_expressions) {
			values.push_back(expression->Evaluate(scope, stack));
		}

		if (m_type == ECompoundType::ARRAY && values.size() > 1) {
			const CValue& firstValue = values.front();
			for (unsigned i = 1; i < values.size(); ++i) {
				if (!CValue::TypesEqual(firstValue, values[i])) {
					throw ExInconsistentTypesInArray(GetLocation(), stack);
				}
			}
		}

		return CValue::MakeCompound(m_type, values);
	}

}
}
