#include "AstReference.h"
#include "AstBind.h"

#include "Exceptions.h"
#include "Scope.h"

namespace moon {
namespace itpr {

	CAstReference::CAstReference(const CSourceLocation& location, std::string symbol) :
		CAstNode{ location },
		m_symbol{ symbol }
	{}

	CValue CAstReference::Evaluate(CScope& scope, CStack& stack) const
	{
		return scope.GetValue(m_symbol, GetLocation(), stack);
	}

	void CAstReference::GetUsedSymbols(std::vector<std::string>& symbols) const
	{
		symbols.push_back(m_symbol);
	}

}
}
