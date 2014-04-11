#ifndef MOON_ITPR_FUNC_DEF_H
#define MOON_ITPR_FUNC_DEF_H

#include <vector>
#include <string>
#include <memory>

#include "AstFunction.h"

namespace moon {
namespace itpr {

	class CAstFuncDef : public CAstFunction {
		const std::vector<std::string> m_formalArgs;
		const std::vector<CSourceLocation> m_argLocations;
		const std::vector<std::unique_ptr<CAstNode>> m_expressions;

	public:
		CAstFuncDef(
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

		template <class Func>
		void ForEachExpression(Func f) const
		{
			for (const auto& expr : m_expressions)
			{

			}
		}

		int GetArgsCount() const
		{
			return m_formalArgs.size();
		}
	};

}
}

#endif
