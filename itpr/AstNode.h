#ifndef MOON_ITPR_AST_NODE_H
#define MOON_ITPR_AST_NODE_H

#include <memory>

#include "../API/Value.h"
#include "../common/SourceLocation.h"

namespace moon {
namespace itpr {

	class CScope;
	class CStack;

	class CAstNode {
		const CSourceLocation m_location;
	public:
		CAstNode(CSourceLocation location) :
			m_location{ location }
		{}

		CSourceLocation GetLocation() const
		{
			return m_location;
		}

		virtual ~CAstNode()	{}

		virtual CValue Evaluate(std::shared_ptr<CScope> scope, CStack& stack) const = 0;
	};

}
}

#endif
