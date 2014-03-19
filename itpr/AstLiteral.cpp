#include "AstLiteral.h"

namespace moon {
namespace itpr {

	CAstLiteral::CAstLiteral(CValue value) :
		m_value{ value }
	{}

	CValue CAstLiteral::Evaluate(CScope&) const
	{
		return m_value;
	}

}
}