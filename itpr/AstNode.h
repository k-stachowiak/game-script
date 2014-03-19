#ifndef MOON_ITPR_AST_NODE_H
#define MOON_ITPR_AST_NODE_H

#include "../API/Value.h"

namespace moon {
namespace itpr {

	class CScope;

	class CAstNode {
	public:
		virtual ~CAstNode()	{}
		virtual CValue Evaluate(CScope& scope) const = 0;
	};

}
}

#endif