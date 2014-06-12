#ifndef MOON_AST_NODE_H
#define MOON_AST_NODE_H

#include "../itpr/Value.h"
#include "../common/SourceLocation.h"

namespace moon {

    namespace itpr {
        class Scope;
        class Stack;
    }

namespace ast {

    

    class AstNode {
        const SourceLocation m_location;
    public:
        AstNode(SourceLocation location) :
            m_location{ location }
        {}

        SourceLocation GetLocation() const
        {
            return m_location;
        }

        virtual ~AstNode()    {}

        virtual itpr::Value Evaluate(itpr::Scope& scope, itpr::Stack& stack) const = 0;
        virtual void GetUsedSymbols(std::vector<std::string>& symbols) const = 0;
    };

}
}

#endif
