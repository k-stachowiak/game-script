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

    itpr::Value AddInteger(long lhs, long rhs) { return itpr::Value::MakeInteger(lhs + rhs); }
    itpr::Value SubInteger(long lhs, long rhs) { return itpr::Value::MakeInteger(lhs - rhs); }
    itpr::Value MulInteger(long lhs, long rhs) { return itpr::Value::MakeInteger(lhs * rhs); }
    itpr::Value DivInteger(long lhs, long rhs) { return itpr::Value::MakeInteger(lhs / rhs); }

    itpr::Value AddReal(double lhs, double rhs) { return itpr::Value::MakeReal(lhs + rhs); }
    itpr::Value SubReal(double lhs, double rhs) { return itpr::Value::MakeReal(lhs - rhs); }
    itpr::Value MulReal(double lhs, double rhs) { return itpr::Value::MakeReal(lhs * rhs); }
    itpr::Value DivReal(double lhs, double rhs) { return itpr::Value::MakeReal(lhs / rhs); }
    
    itpr::Value SqrtInteger(long x) { return itpr::Value::MakeInteger(static_cast<long>(sqrt(x))); }
    itpr::Value SqrtReal(double x)  { return itpr::Value::MakeReal(sqrt(x)); }

    // AST part implementation.
    // ========================

    itpr::Value AstUnaryArithmeticBif::Execute(itpr::Scope& scope, itpr::Stack& stack) const
    {
        itpr::Value actualArgument = scope.GetValue("x", GetLocation(), stack);

        switch (actualArgument.GetType()) {
        case itpr::ValueType::INTEGER:
            return m_integerImplementation(actualArgument.GetInteger());

        case itpr::ValueType::REAL:
            return m_realImplementation(actualArgument.GetReal());

        default:
            throw itpr::ExAstArithmeticTypeMismatch{
                SourceLocation::MakeBuiltInFunction(),
                stack
            };
        }
    }

    itpr::Value AstBinaryArithmeticBif::Execute(itpr::Scope& scope, itpr::Stack& stack) const
    {
        itpr::Value rhs = scope.GetValue("rhs", GetLocation(), stack);
        itpr::Value lhs = scope.GetValue("lhs", GetLocation(), stack);

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
                SourceLocation::MakeBuiltInFunction(),
                stack
            };
        }        
    }

    std::vector<std::pair<std::string, std::unique_ptr<AstNode>>> BuildBifMap()
    {
        std::vector<std::pair<std::string, std::unique_ptr<AstNode>>> result;

        result.push_back(std::make_pair("+", std::unique_ptr<AstNode> { new AstBinaryArithmeticBif{ AddInteger, AddReal }}));
        result.push_back(std::make_pair("-", std::unique_ptr<AstNode> { new AstBinaryArithmeticBif{ SubInteger, SubReal }}));
        result.push_back(std::make_pair("*", std::unique_ptr<AstNode> { new AstBinaryArithmeticBif{ MulInteger, MulReal }}));
        result.push_back(std::make_pair("/", std::unique_ptr<AstNode> { new AstBinaryArithmeticBif{ DivInteger, DivReal }}));

        result.push_back(std::make_pair("sqrt", std::unique_ptr<AstNode> { new AstUnaryArithmeticBif{ SqrtInteger, SqrtReal }}));

        return result;
    }

}
}
}
