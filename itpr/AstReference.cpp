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

	CValue CAstReference::Evaluate(CScope& scope, CStack&) const
	{
		return scope.GetValue(m_symbol);
	}

}
}
