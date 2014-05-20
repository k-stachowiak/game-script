#include <cmath>

#include "../itpr/Value.h"
#include "../itpr/Exceptions.h"
#include "../itpr/Scope.h"
#include "Bind.h"
#include "Bif.h"

namespace moon {
namespace ast {
namespace bif {

    // Build-in functions implementation.
    // ==================================

    itpr::CValue AddInteger(long lhs, long rhs) { return itpr::CValue::MakeInteger(lhs + rhs); }
    itpr::CValue SubInteger(long lhs, long rhs) { return itpr::CValue::MakeInteger(lhs - rhs); }
    itpr::CValue MulInteger(long lhs, long rhs) { return itpr::CValue::MakeInteger(lhs * rhs); }
    itpr::CValue DivInteger(long lhs, long rhs) { return itpr::CValue::MakeInteger(lhs / rhs); }

    itpr::CValue AddReal(double lhs, double rhs) { return itpr::CValue::MakeReal(lhs + rhs); }
    itpr::CValue SubReal(double lhs, double rhs) { return itpr::CValue::MakeReal(lhs - rhs); }
    itpr::CValue MulReal(double lhs, double rhs) { return itpr::CValue::MakeReal(lhs * rhs); }
    itpr::CValue DivReal(double lhs, double rhs) { return itpr::CValue::MakeReal(lhs / rhs); }
    
    itpr::CValue SqrtInteger(long x) { return itpr::CValue::MakeInteger(static_cast<long>(sqrt(x))); }
    itpr::CValue SqrtReal(double x)  { return itpr::CValue::MakeReal(sqrt(x)); }

    // AST part implementation.
    // ========================

    itpr::CValue CAstUnaryArithmeticBif::Execute(itpr::CScope& scope, itpr::CStack& stack) const
    {
        itpr::CValue actualArgument = scope.GetValue("x", GetLocation(), stack);

        switch (actualArgument.GetType()) {
        case itpr::EValueType::INTEGER:
            return m_integerImplementation(actualArgument.GetInteger());

        case itpr::EValueType::REAL:
            return m_realImplementation(actualArgument.GetReal());

        default:
            throw itpr::ExAstArithmeticTypeMismatch{
                CSourceLocation::MakeBuiltInFunction(),
                stack
            };
        }
    }

    itpr::CValue CAstBinaryArithmeticBif::Execute(itpr::CScope& scope, itpr::CStack& stack) const
    {
        itpr::CValue rhs = scope.GetValue("rhs", GetLocation(), stack);
        itpr::CValue lhs = scope.GetValue("lhs", GetLocation(), stack);

        if (IsInteger(lhs) && IsInteger(rhs)) {
            return m_integerImplementation(lhs.GetInteger(), rhs.GetInteger());

        } else if (IsInteger(lhs) && IsReal(rhs)) {
            return m_realImplementation(static_cast<double>(lhs.GetInteger()), rhs.GetReal());

        } else if (IsReal(lhs) && IsInteger(rhs)) {
            return m_realImplementation(lhs.GetReal(), static_cast<double>(rhs.GetInteger()));

        } else if (IsReal(lhs) && IsReal(rhs)) {
            return m_realImplementation(lhs.GetReal(), rhs.GetReal());

        } else {
            throw itpr::ExAstArithmeticTypeMismatch{
                CSourceLocation::MakeBuiltInFunction(),
                stack
            };
        }        
    }

    std::vector<std::pair<std::string, std::unique_ptr<CAstNode>>> BuildBifMap()
    {
        std::vector<std::pair<std::string, std::unique_ptr<CAstNode>>> result;

        result.push_back(std::make_pair("+", std::unique_ptr<CAstNode> { new CAstBinaryArithmeticBif{ AddInteger, AddReal }}));
        result.push_back(std::make_pair("-", std::unique_ptr<CAstNode> { new CAstBinaryArithmeticBif{ SubInteger, SubReal }}));
        result.push_back(std::make_pair("*", std::unique_ptr<CAstNode> { new CAstBinaryArithmeticBif{ MulInteger, MulReal }}));
        result.push_back(std::make_pair("/", std::unique_ptr<CAstNode> { new CAstBinaryArithmeticBif{ DivInteger, DivReal }}));

        result.push_back(std::make_pair("sqrt", std::unique_ptr<CAstNode> { new CAstUnaryArithmeticBif{ SqrtInteger, SqrtReal }}));

        return result;
    }

}
}
}
