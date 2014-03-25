#ifndef MOON_ITPR_AST_BIND_H
#define MOON_ITPR_AST_BIND_H

#include <memory>

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
			int line, 
			int column, 
			std::string symbol, 
			std::unique_ptr<CAstNode>&& expression);

		CValue Evaluate(CScope& scope) const override;
		const std::string& GetSymbol() const { return m_symbol; }
		const CAstFunction* TryGettingFuncDecl() const;
		const CAstNode* TryGettingNonFuncDecl() const;
		std::unique_ptr<CAstNode> TakeOverExpression();
	};

}
}

#endif
