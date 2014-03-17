#ifndef MOON_ITPR_AST_NODE_H
#define MOON_ITPR_AST_NODE_H

#include "Value.h"
#include "Scope.h"

namespace moon {
namespace itpr {

	class CAstNode
	{
	public:
		virtual ~CAstNode()
		{
		}

		virtual CValue Evaluate(CScope& scope) = 0;
	};

}
}

#endif