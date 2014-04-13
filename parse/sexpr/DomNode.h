#ifndef MOON_PARSE_SEXPR_DOM_NODE_H
#define MOON_PARSE_SEXPR_DOM_NODE_H

#include <string>
#include <vector>

#include "../../common/SourceLocation.h"

namespace moon {
namespace parse {
namespace sexpr {

	enum class EDomNodeType {
		ATOM,
		COMPOUND
	};

	enum class EDomCompoundType {
		CORE,
		ARRAY,
		TUPLE
	};

	class CDomNode
	{
		const CSourceLocation m_location;

		EDomNodeType m_type;		
		std::string m_atom;
		EDomCompoundType m_compoundType;
		std::vector<CDomNode> m_compoundChildren;

		CDomNode(
			const CSourceLocation& location,
			EDomNodeType type,
			std::string atom,
			EDomCompoundType compoundType,
			std::vector<CDomNode> compoundChildren) :
			m_location{ location },
			m_type{ type },
			m_atom{ atom },
			m_compoundType{ compoundType },
			m_compoundChildren{ compoundChildren }
		{}

	public:
		const CSourceLocation& GetLocation() const
		{
			return m_location;
		}

		friend bool operator==(const CDomNode& node, const std::string& str)
		{
			return
				node.IsAtom() &&
				node.GetAtom() == str;
		}

		friend bool operator!=(const CDomNode& node, const std::string& str)
		{
			return !(node == str);
		}

		bool IsAtom() const
		{
			return m_type == EDomNodeType::ATOM;
		}

		bool IsAtom(const std::string& atom) const {
			return IsAtom() && m_atom == atom;
		}

		const std::string& GetAtom() const {
			return m_atom;
		}

		bool IsCompound() const
		{
			return m_type == EDomNodeType::COMPOUND;
		}

		bool IsCompoundCore() const
		{
			return IsCompound() && m_compoundType == EDomCompoundType::CORE;
		}

		bool IsCompoundArray() const
		{
			return IsCompound() && m_compoundType == EDomCompoundType::ARRAY;
		}

		bool IsCompoundTuple() const
		{
			return IsCompound() && m_compoundType == EDomCompoundType::TUPLE;
		}

		EDomCompoundType GetType() const
		{
			return m_compoundType;
		}

		std::vector<CDomNode>::const_iterator ChildrenBegin() const
		{
			return m_compoundChildren.cbegin();
		}

		std::vector<CDomNode>::const_iterator ChildrenEnd() const
		{
			return m_compoundChildren.cend();
		}

		static CDomNode MakeAtom(const CSourceLocation& location, const std::string& atom)
		{
			return CDomNode{
				location,
				EDomNodeType::ATOM,
				atom,
				EDomCompoundType::CORE,
				{}
			};
		}

		static CDomNode MakeCompound(
			const CSourceLocation& location,
			EDomCompoundType compoundType,
			const std::vector<CDomNode>& children)
		{
			return CDomNode{
				location,
				EDomNodeType::COMPOUND,
				{},
				compoundType,
				children
			};
		}
	};

}
}
}

#endif
