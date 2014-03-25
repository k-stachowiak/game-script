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
		const std::vector<std::pair<int, int>> m_argLocations;
		const std::vector<std::unique_ptr<CAstNode>> m_expressions;

	public:
		CAstFuncDecl(
			int line,
			int column,
			std::vector<std::string> formalArgs,
			std::vector<std::pair<int, int>> argLocations,
			std::vector<std::unique_ptr<CAstNode>>&& expressions);

		CValue Evaluate(CScope& scope) const override;

		const std::vector<std::string>& GetFormalArgs() const override
		{
			return m_formalArgs;
		}

		const std::vector<std::pair<int, int>>& GetArgLocations() const override
		{
			return m_argLocations;
		}
	};

}
}

#endif
