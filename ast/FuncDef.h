#ifndef MOON_AST_FUNC_DEF_H
#define MOON_AST_FUNC_DEF_H

#include <vector>
#include <string>
#include <memory>

#include "Function.h"

namespace moon {
namespace ast {

	class CAstFuncDef : public CAstFunction {
		const std::vector<std::unique_ptr<CAstNode>> m_expressions;

	public:
		CAstFuncDef(
			const CSourceLocation& location,
			std::vector<std::string> formalArgs,
			std::vector<CSourceLocation> argLocations,
			std::vector<std::unique_ptr<CAstNode>>&& expressions);

		CValue Execute(itpr::CScope& scope, itpr::CStack& stack) const override;
		void GetUsedSymbols(std::vector<std::string>& symbols) const override;
	};

}
}

#endif
