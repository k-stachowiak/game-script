#ifndef MOON_PARSE_SEXPR_DOM_NODE_H
#define MOON_PARSE_SEXPR_DOM_NODE_H

#include <string>
#include <vector>

namespace moon {
namespace parse {

	enum class ESexprDomNodeType {
		ATOM,
		COMPOUND
	};

	enum class ESexprDomCompoundType {
		CPD_CORE,
		CPD_LIST,
		CPD_ARRAY,
		CPD_TUPLE
	};

	class CSexprDomNode
	{
		ESexprDomNodeType m_type;		
		std::string m_atom;
		ESexprDomCompoundType m_compoundType;
		std::vector<CSexprDomNode> m_compoundChildren;

		CSexprDomNode(
			ESexprDomNodeType type,
			std::string atom,
			ESexprDomCompoundType compoundType,
			std::vector<CSexprDomNode> compoundChildren) :
			m_type(type),
			m_atom(atom),
			m_compoundType(compoundType),
			m_compoundChildren(compoundChildren)
		{}

	public:
		bool IsAtom() const
		{
			return m_type == ESexprDomNodeType::ATOM;
		}

		bool IsAtom(const std::string& atom) const {
			return IsAtom() && m_atom == atom;
		}

		const std::string GetAtom() const {
			return m_atom;
		}

		bool IsCompound() const
		{
			return m_type == ESexprDomNodeType::COMPOUND;
		}

		bool IsCompoundCore() const
		{
			return IsCompound() && m_compoundType == ESexprDomCompoundType::CPD_CORE;
		}

		bool IsCompoundArray() const
		{
			return IsCompound() && m_compoundType == ESexprDomCompoundType::CPD_ARRAY;
		}

		bool IsCompoundList() const
		{
			return IsCompound() && m_compoundType == ESexprDomCompoundType::CPD_LIST;
		}

		bool IsCompoundTuple() const
		{
			return IsCompound() && m_compoundType == ESexprDomCompoundType::CPD_TUPLE;
		}

		std::vector<CSexprDomNode>::const_iterator ChildrenBegin() const
		{
			return m_compoundChildren.cbegin();
		}

		std::vector<CSexprDomNode>::const_iterator ChildrenEnd() const
		{
			return m_compoundChildren.cend();
		}

		static CSexprDomNode MakeAtom(const std::string& atom)
		{
			return CSexprDomNode{
				ESexprDomNodeType::ATOM,
				atom,
				ESexprDomCompoundType::CPD_CORE,
				{}
			};
		}

		static CSexprDomNode MakeCompound(
			ESexprDomCompoundType compoundType,
			const std::vector<CSexprDomNode>& children)
		{
			return CSexprDomNode{
				ESexprDomNodeType::COMPOUND,
				{},
				compoundType,
				children
			};
		}
	};

}
}

#endif