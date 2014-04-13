#ifndef MOON_ITPR_FUNC_DEF_H
#define MOON_ITPR_FUNC_DEF_H

#include <vector>
#include <string>
#include <memory>

#include "AstFunction.h"

namespace moon {
namespace itpr {

	class CAstFuncDef : public CAstFunction {
		const std::vector<std::unique_ptr<CAstNode>> m_expressions;

	public:
		CAstFuncDef(
			const CSourceLocation& location,
			std::vector<std::string> formalArgs,
			std::vector<CSourceLocation> argLocations,
			std::vector<std::unique_ptr<CAstNode>>&& expressions);

		CValue Execute(CScope& scope, CStack& stack) const override;
	};

}
}

#endif
