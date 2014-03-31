#ifndef MOON_ITPR_FUNC_DECL_H
#define MOON_ITPR_FUNC_DECL_H

#include <vector>
#include <string>
#include <memory>

#include "AstFunction.h"

namespace moon {
namespace itpr {

	class CAstFuncDecl : public CAstFunction {
		const std::vector<std::string> m_formalArgs;
		const std::vector<CSourceLocation> m_argLocations;
		const std::vector<std::unique_ptr<CAstNode>> m_expressions;

	public:
		CAstFuncDecl(
			const CSourceLocation& location,
			std::vector<std::string> formalArgs,
			std::vector<CSourceLocation> argLocations,
			std::vector<std::unique_ptr<CAstNode>>&& expressions);

		CValue Evaluate(CScope& scope, CStack& stack) const override;

		const std::vector<std::string>& GetFormalArgs() const override
		{
			return m_formalArgs;
		}

		const std::vector<CSourceLocation>& GetArgLocations() const override
		{
			return m_argLocations;
		}
	};

}
}

#endif
