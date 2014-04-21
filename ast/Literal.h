#ifndef MOON_AST_LITERAL_H
#define MOON_AST_LITERAL_H

#include "Node.h"

namespace moon {
namespace ast {

	class CAstLiteral : public CAstNode {
		const CValue m_value;

	public:
		CAstLiteral(const CSourceLocation& location, CValue value);
		CValue Evaluate(itpr::CScope&, itpr::CStack&) const override;
		void GetUsedSymbols(std::vector<std::string>&) const override {}
	};

}
}

#endif
