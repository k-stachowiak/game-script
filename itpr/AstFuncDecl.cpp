#include "AstFuncDecl.h"

namespace moon {
namespace itpr {

	CAstFuncDecl::CAstFuncDecl(
		int line,
		int column,
		std::vector<std::string> formalArgs,
		std::vector<std::pair<int, int>> argLocations,
		std::vector<std::unique_ptr<CAstNode>>&& expressions) :
		CAstFunction{ line, column },
		m_formalArgs{ formalArgs },
		m_argLocations{ argLocations },
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
