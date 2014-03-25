#ifndef MOON_ITPR_AST_LITERAL_H
#define MOON_ITPR_AST_LITERAL_H

#include "AstNode.h"

namespace moon {
namespace itpr {

	class CAstLiteral : public CAstNode {
		const CValue m_value;

	public:
		CAstLiteral(int line, int column, CValue value);
		CValue Evaluate(CScope& scope) const override;
	};

}
}

#endif