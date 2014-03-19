#include "AstCompound.h"

namespace moon {
namespace itpr {

	CAstCompound::CAstCompound(ECompoundType type, std::vector<std::unique_ptr<CAstNode>>&& expressions) :
		m_type{ type },
		m_expressions{ std::move(expressions) }
	{}

	CValue CAstCompound::Evaluate(CScope& scope) const
	{
		std::vector<CValue> values;
		for (const auto& expression : m_expressions) {
			values.push_back(expression->Evaluate(scope));
		}
		return CValue::MakeCompound(m_type, values);
	}

}
}