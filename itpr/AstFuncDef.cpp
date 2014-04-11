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

	CValue CAstFuncDef::Evaluate(CScope& scope, CStack& stack) const
	{
		// TODO: WTF is this??? This should rather create a function value!
		// TODO: Allow for evaluation of function declarations.
		CValue result;
		for (const auto& expression : m_expressions) {
			result = expression->Evaluate(scope, stack);
		}
		return result;
	}

}
}
