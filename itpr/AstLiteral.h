#ifndef MOON_ITPR_AST_LITERAL_H
#define MOON_ITPR_AST_LITERAL_H

#include "AstNode.h"

namespace moon {
namespace itpr {

	class CAstLiteral : public CAstNode {
		const CValue m_value;

	public:
		CAstLiteral(const CSourceLocation& location, CValue value);
		CValue Evaluate(CScope&, CStack&) const override;
		void GetUsedSymbols(std::vector<std::string>&) const override {}
	};

}
}

#endif
