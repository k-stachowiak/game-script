#ifndef MOON_ITPR_AST_REFERENCE_H
#define MOON_ITPR_AST_REFERENCE_H

#include "AstNode.h"

namespace moon {
namespace itpr {

	class CAstReference : public CAstNode {
		const std::string m_symbol;

	public:
		CAstReference(const CSourceLocation& location, std::string symbol);
		CValue Evaluate(CScope& scope, CStack&) const override;
		void GetUsedSymbols(std::vector<std::string>& symbols) const override;
	};

}
}

#endif
