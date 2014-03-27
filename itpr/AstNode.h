#ifndef MOON_ITPR_AST_NODE_H
#define MOON_ITPR_AST_NODE_H

#include "../API/Value.h"
#include "../API/SourceLocation.h"

namespace moon {
namespace itpr {

	class CScope;

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

		virtual CValue Evaluate(CScope& scope) const = 0;
	};

}
}

#endif
