#include "AstLiteral.h"

namespace moon {
namespace itpr {

	CAstLiteral::CAstLiteral(int line, int column, CValue value) :
		CAstNode{ line, column },
		m_value{ value }
	{}

	CValue CAstLiteral::Evaluate(CScope&) const
	{
		return m_value;
	}

}
}