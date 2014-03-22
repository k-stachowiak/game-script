#include "AstBind.h"
#include "AstLiteral.h"

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
		scope.RegisterBind(m_symbol, std::unique_ptr<CAstNode> {new CAstLiteral{ result }});
		return result;
	}

	const CAstFuncDecl* CAstBind::TryGettingFuncDecl() const
	{
		return dynamic_cast<const CAstFuncDecl*>(m_expression.get());
	}
}
}
