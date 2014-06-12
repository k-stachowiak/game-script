#include "Literal.h"

namespace moon {
namespace ast {

    AstLiteral::AstLiteral(const SourceLocation& location, itpr::Value value) :
        AstNode{ location },
        m_value{ value }
    {}

    itpr::Value AstLiteral::Evaluate(itpr::Scope&, itpr::Stack&) const
    {
        return m_value;
    }

}
}
