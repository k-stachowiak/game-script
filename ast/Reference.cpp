#include "Reference.h"
#include "Bind.h"

#include "../itpr/Scope.h"

namespace moon {
namespace ast {

	CAstReference::CAstReference(const CSourceLocation& location, std::string symbol) :
		CAstNode{ location },
		m_symbol{ symbol }
	{}

	CValue CAstReference::Evaluate(itpr::CScope& scope, itpr::CStack& stack) const
	{
		return scope.GetValue(m_symbol, GetLocation(), stack);
	}

	void CAstReference::GetUsedSymbols(std::vector<std::string>& symbols) const
	{
		symbols.push_back(m_symbol);
	}

}
}
