#ifndef MOON_AST_REFERENCE_H
#define MOON_AST_REFERENCE_H

#include "Node.h"

namespace moon {
namespace ast {

	class CAstReference : public CAstNode {
		const std::string m_symbol;

	public:
		CAstReference(const CSourceLocation& location, std::string symbol);
		CValue Evaluate(itpr::CScope& scope, itpr::CStack& stack) const override;
		void GetUsedSymbols(std::vector<std::string>& symbols) const override;
	};

}
}

#endif
