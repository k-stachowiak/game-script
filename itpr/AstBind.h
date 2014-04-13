#ifndef MOON_ITPR_AST_BIND_H
#define MOON_ITPR_AST_BIND_H

#include <memory>

#include "../common/SourceLocation.h"
#include "AstNode.h"
#include "AstFunction.h"

namespace moon {
namespace itpr {
		
	class CAstBind : public CAstNode {
		const std::string m_symbol;
		std::unique_ptr<CAstNode> m_expression;

	public:
		CAstBind(
			const CSourceLocation& location, 
			std::string symbol, 
			std::unique_ptr<CAstNode>&& expression);

		CValue Evaluate(std::shared_ptr<CScope> scope, CStack& stack) const override;
		const std::string& GetSymbol() const { return m_symbol; }
		const CAstNode& GetExpression() const;
		std::unique_ptr<CAstNode> TakeOverExpression();
	};

}
}

#endif
