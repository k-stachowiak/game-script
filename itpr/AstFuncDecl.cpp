#include "AstFuncDecl.h"

namespace moon {
namespace itpr {

	CAstFuncDecl::CAstFuncDecl(
		const std::vector<std::string> formalArgs,
		std::vector<std::unique_ptr<CAstNode>>&& expressions) :
		m_formalArgs{ formalArgs },
		m_expressions{ std::move(expressions) }
	{}

	CValue CAstFuncDecl::Evaluate(CScope& scope) const
	{
		CValue result;
		for (const auto& expression : m_expressions) {
			result = expression->Evaluate(scope);
		}
		return result;
	}

}
}
