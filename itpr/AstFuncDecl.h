#ifndef MOON_ITPR_FUNC_DECL_H
#define MOON_ITPR_FUNC_DECL_H

#include <vector>
#include <string>

#include "AstNode.h"

namespace moon {
namespace itpr {

	class CAstFuncDecl : public CAstNode
	{
	public:
		CValue Evaluate(CScope& scope) override;

		const std::vector<std::string>& GetFormalArgs() const;
	};

}
}

#endif