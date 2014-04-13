#include "AstLiteral.h"

namespace moon {
namespace itpr {

	CAstLiteral::CAstLiteral(const CSourceLocation& location, CValue value) :
		CAstNode{ location },
		m_value{ value }
	{}

	CValue CAstLiteral::Evaluate(std::shared_ptr<CScope>, CStack&) const
	{
		return m_value;
	}

}
}
