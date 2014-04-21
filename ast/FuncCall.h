#ifndef MOON_AST_FUNC_CALL_H
#define MOON_AST_FUNC_CALL_H

#include <memory>

#include "Node.h"

namespace moon {
namespace ast {

	class CAstFuncCall : public CAstNode {
		const std::string m_symbol;
		const std::vector<std::unique_ptr<CAstNode>> m_actualArgs;

	public:
		CAstFuncCall(
			const CSourceLocation& location,
			std::string symbol, 
			std::vector<std::unique_ptr<CAstNode>>&& actualArgs);

		CValue Evaluate(itpr::CScope& scope, itpr::CStack& stack) const override;
		void GetUsedSymbols(std::vector<std::string>& symbols) const override;
	};

}
}

#endif
