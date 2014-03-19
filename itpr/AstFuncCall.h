#ifndef MOON_ITPR_AST_FUNC_CALL_H
#define MOON_ITPR_AST_FUNC_CALL_H

#include <memory>

#include "AstNode.h"

namespace moon {
namespace itpr {

	class CAstFuncCall : public CAstNode {
		const std::string m_symbol;
		const std::vector<std::unique_ptr<CAstNode>> m_actualArgs;

	public:
		CAstFuncCall(std::string symbol, std::vector<std::unique_ptr<CAstNode>>&& actualArgs);
		CValue Evaluate(CScope& scope) const override;
	};

}
}

#endif