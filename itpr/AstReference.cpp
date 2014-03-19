#include "AstReference.h"

#include "Scope.h"

namespace moon {
namespace itpr {

	CAstReference::CAstReference(std::string symbol) :
		m_symbol{ symbol }
	{}

	CValue CAstReference::Evaluate(CScope& scope) const
	{
		return scope.GetBind(m_symbol);
	}

}
}