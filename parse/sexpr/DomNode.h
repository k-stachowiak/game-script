#ifndef MOON_PARSE_SEXPR_DOM_NODE_H
#define MOON_PARSE_SEXPR_DOM_NODE_H

#include <string>
#include <vector>

#include "../../common/SourceLocation.h"

namespace moon {
namespace parse {
namespace sexpr {

    enum class DomNodeType {
        ATOM,
        COMPOUND
    };

    enum class DomCompoundType {
        CORE,
        ARRAY,
        TUPLE
    };

    class DomNode
    {
        const SourceLocation m_location;

        DomNodeType m_type;        
        std::string m_atom;
        DomCompoundType m_compoundType;
        std::vector<DomNode> m_compoundChildren;

        DomNode(
            const SourceLocation& location,
            DomNodeType type,
            std::string atom,
            DomCompoundType compoundType,
            std::vector<DomNode> compoundChildren) :
            m_location{ location },
            m_type{ type },
            m_atom{ atom },
            m_compoundType{ compoundType },
            m_compoundChildren{ compoundChildren }
        {}

    public:
        const SourceLocation& GetLocation() const
        {
            return m_location;
        }

        friend bool operator==(const DomNode& node, const std::string& str)
        {
            return
                node.IsAtom() &&
                node.GetAtom() == str;
        }

        friend bool operator!=(const DomNode& node, const std::string& str)
        {
            return !(node == str);
        }

        bool IsAtom() const
        {
            return m_type == DomNodeType::ATOM;
        }

        bool IsAtom(const std::string& atom) const {
            return IsAtom() && m_atom == atom;
        }

        const std::string& GetAtom() const {
            return m_atom;
        }

        bool IsCompound() const
        {
            return m_type == DomNodeType::COMPOUND;
        }

        bool IsCompoundCore() const
        {
            return IsCompound() && m_compoundType == DomCompoundType::CORE;
        }

        bool IsCompoundArray() const
        {
            return IsCompound() && m_compoundType == DomCompoundType::ARRAY;
        }

        bool IsCompoundTuple() const
        {
            return IsCompound() && m_compoundType == DomCompoundType::TUPLE;
        }

        DomCompoundType GetType() const
        {
            return m_compoundType;
        }

        std::vector<DomNode>::const_iterator ChildrenBegin() const
        {
            return m_compoundChildren.cbegin();
        }

        std::vector<DomNode>::const_iterator ChildrenEnd() const
        {
            return m_compoundChildren.cend();
        }

        static DomNode MakeAtom(const SourceLocation& location, const std::string& atom)
        {
            return DomNode{
                location,
                DomNodeType::ATOM,
                atom,
                DomCompoundType::CORE,
                {}
            };
        }

        static DomNode MakeCompound(
            const SourceLocation& location,
            DomCompoundType compoundType,
            const std::vector<DomNode>& children)
        {
            return DomNode{
                location,
                DomNodeType::COMPOUND,
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
