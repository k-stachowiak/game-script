#include <cassert>
#include "AstFuncDef.h"

namespace moon {
namespace itpr {

	CAstFuncDef::CAstFuncDef(
		const CSourceLocation& location,
		std::vector<std::string> formalArgs,
		std::vector<CSourceLocation> argLocations,
		std::vector<std::unique_ptr<CAstNode>>&& expressions) :
		CAstFunction{ location },
		m_formalArgs{ formalArgs },
		m_argLocations{ argLocations },
		m_expressions{ std::move(expressions) }
	{
		assert(m_formalArgs.size() == m_argLocations.size());
	}

	CValue CAstFuncDef::Execute(CScope& scope, CStack& stack) const
	{
		CValue result;
		for (const auto& expression : m_expressions) {
			result = expression->Evaluate(scope, stack);
		}
		return result;
	}

}
}
