#include "Literal.h"

namespace moon {
namespace ast {

    CAstLiteral::CAstLiteral(const CSourceLocation& location, itpr::CValue value) :
        CAstNode{ location },
        m_value{ value }
    {}

    itpr::CValue CAstLiteral::Evaluate(itpr::CScope&, itpr::CStack&) const
    {
        return m_value;
    }

}
}
