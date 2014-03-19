#ifndef MOON_ITPR_FUNC_DECL_H
#define MOON_ITPR_FUNC_DECL_H

#include <vector>
#include <string>
#include <memory>

#include "AstNode.h"

namespace moon {
namespace itpr {

	class CAstFuncDecl : public CAstNode {
		const std::vector<std::string> m_formalArgs;
		const std::vector<std::unique_ptr<CAstNode>> m_expressions;

	public:
		CAstFuncDecl(
			const std::vector<std::string> formalArgs,
			std::vector<std::unique_ptr<CAstNode>>&& expressions);

		CValue Evaluate(CScope& scope) const override;

		const std::vector<std::string>& GetFormalArgs() const
		{
			return m_formalArgs;
		}
	};

}
}

#endif