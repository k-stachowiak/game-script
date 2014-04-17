#ifndef MOON_ITPR_AST_COMPOUND_H
#define MOON_ITPR_AST_COMPOUND_H

#include <memory>

#include "AstNode.h"

namespace moon {
namespace itpr {

	class CAstCompound : public CAstNode {
		const ECompoundType m_type;
		const std::vector<std::unique_ptr<CAstNode>> m_expressions;

	public:
		CAstCompound(
			const CSourceLocation& location,
			ECompoundType type, 
			std::vector<std::unique_ptr<CAstNode>>&& expressions);

		CValue Evaluate(CScope& scope, CStack& stack) const override;
		void GetUsedSymbols(std::vector<std::string>& symbols) const override;
	};

}
}

#endif
