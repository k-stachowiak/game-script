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
		// TODO: Bind expression shouldn't be a bind

	public:
		CAstBind(
			const CSourceLocation& location, 
			std::string symbol, 
			std::unique_ptr<CAstNode>&& expression);

		CValue Evaluate(CScope& scope) const override;
		const std::string& GetSymbol() const { return m_symbol; }
		const CAstFunction* TryGettingFunction() const;
		const CAstNode* TryGettingNonFunction() const;
		std::unique_ptr<CAstNode> TakeOverExpression();
	};

}
}

#endif
