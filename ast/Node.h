#ifndef MOON_AST_NODE_H
#define MOON_AST_NODE_H

#include "../API/Value.h"
#include "../common/SourceLocation.h"

namespace moon {

	namespace itpr {
		class CScope;
		class CStack;
	}

namespace ast {

	

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

		virtual CValue Evaluate(itpr::CScope& scope, itpr::CStack& stack) const = 0;
		virtual void GetUsedSymbols(std::vector<std::string>& symbols) const = 0;
	};

}
}

#endif
