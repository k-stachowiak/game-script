#ifndef MOON_ITPR_AST_REFERENCE_H
#define MOON_ITPR_AST_REFERENCE_H

#include "AstNode.h"

namespace moon {
namespace itpr {

	class CAstReference : public CAstNode {
		const std::string m_symbol;

	public:
		CAstReference(std::string symbol);
		CValue Evaluate(CScope& scope) const override;
	};

}
}

#endif