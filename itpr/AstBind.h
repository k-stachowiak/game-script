#ifndef MOON_ITPR_AST_BIND_H
#define MOON_ITPR_AST_BIND_H

#include <memory>

#include "AstNode.h"

namespace moon {
namespace itpr {
		
	class CAstBind : public CAstNode {
		const std::string m_symbol;
		const std::unique_ptr<CAstNode> m_expression;

	public:
		CAstBind(std::string symbol, std::unique_ptr<CAstNode>&& expression);
		CValue Evaluate(CScope& scope) const override;
	};

}
}

#endif