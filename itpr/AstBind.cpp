#include "AstBind.h"

#include "Scope.h"

namespace moon {
namespace itpr {

	CAstBind::CAstBind(std::string symbol, std::unique_ptr<CAstNode>&& expression) :
		m_symbol{ symbol },
		m_expression{ std::move(expression) }
	{}

	CValue CAstBind::Evaluate(CScope& scope) const
	{
		CValue result = m_expression->Evaluate(scope);
		scope.RegisterValue(m_symbol, result);
		return result;
	}

}
}
