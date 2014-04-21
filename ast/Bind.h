#ifndef MOON_AST_BIND_H
#define MOON_AST_BIND_H

#include <memory>

#include "../common/SourceLocation.h"
#include "Node.h"
#include "Function.h"

namespace moon {
namespace ast {
		
	class CAstBind : public CAstNode {
		const std::string m_symbol;
		std::unique_ptr<CAstNode> m_expression;

	public:
		CAstBind(
			const CSourceLocation& location, 
			std::string symbol, 
			std::unique_ptr<CAstNode>&& expression);

		CValue Evaluate(itpr::CScope& scope, itpr::CStack& stack) const override;
		void GetUsedSymbols(std::vector<std::string>& symbols) const override;
		const std::string& GetSymbol() const { return m_symbol; }
		const CAstNode& GetExpression() const;
		std::unique_ptr<CAstNode> TakeOverExpression();
	};

}
}

#endif
