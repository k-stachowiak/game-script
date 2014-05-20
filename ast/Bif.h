#ifndef MOON_AST_BIF_H
#define MOON_AST_BIF_H

#include <map>
#include <array>
#include <string>
#include <memory>
#include <functional>

#include "Function.h"

namespace moon {
namespace ast {
namespace bif {

    // Note:
    // The "arithmetic" attribute means that there is some
    // argument type conversion magic happening if the argument
    // type doesn't match the ewquired one but is also numeric.

    class CAstUnaryArithmeticBif : public CAstFunction {
        itpr::CValue(*m_integerImplementation)(long);
        itpr::CValue(*m_realImplementation)(double);

    public:
        CAstUnaryArithmeticBif(
            itpr::CValue(integerImplementation)(long),
            itpr::CValue(realImplementation)(double)) :
            CAstFunction{
                CSourceLocation::MakeBuiltInFunction(),
                { "x" },
                { CSourceLocation::MakeBuiltInFunction() }
            },
            m_integerImplementation{ integerImplementation },
            m_realImplementation{ realImplementation }
        {}

        void GetUsedSymbols(std::vector<std::string>&) const override {}
        itpr::CValue Execute(itpr::CScope& scope, itpr::CStack& stack) const override;
    };

    class CAstBinaryArithmeticBif : public CAstFunction {
        std::vector<std::string> m_formalArgs;
        std::vector<CSourceLocation> m_argLocations;
        itpr::CValue(*m_integerImplementation)(long, long);
        itpr::CValue(*m_realImplementation)(double, double);

    public:
        CAstBinaryArithmeticBif(
            itpr::CValue(integerImplementation)(long, long),
            itpr::CValue(realImplementation)(double, double)) :
            CAstFunction{
                CSourceLocation::MakeBuiltInFunction(),
                    { "lhs", "rhs" },
                    { CSourceLocation::MakeBuiltInFunction(),
                      CSourceLocation::MakeBuiltInFunction() }
            },
            m_integerImplementation{ integerImplementation },
            m_realImplementation{ realImplementation }
        {}

        void GetUsedSymbols(std::vector<std::string>&) const {}
        itpr::CValue Execute(itpr::CScope& scope, itpr::CStack& stack) const override;
    };

    std::vector<std::pair<std::string, std::unique_ptr<CAstNode>>> BuildBifMap();

}
}
}

#endif
