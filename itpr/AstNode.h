#ifndef MOON_ITPR_AST_NODE_H
#define MOON_ITPR_AST_NODE_H

#include "../API/Value.h"

namespace moon {
namespace itpr {

	class CScope;

	class CAstNode {
		const int m_line;
		const int m_column;
	public:
		CAstNode(int line, int column) :
			m_line{ line },
			m_column{ column }
		{}

		int GetLine() const
		{
			return m_line;
		}

		int GetColumn() const
		{
			return m_column;
		}

		virtual ~CAstNode()	{}

		virtual CValue Evaluate(CScope& scope) const = 0;
	};

}
}

#endif