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

    class AstUnaryArithmeticBif : public AstFunction {
        itpr::Value(*m_integerImplementation)(long);
        itpr::Value(*m_realImplementation)(double);

    public:
        AstUnaryArithmeticBif(
            itpr::Value(integerImplementation)(long),
            itpr::Value(realImplementation)(double)) :
            AstFunction{
                SourceLocation::MakeBuiltInFunction(),
                { "x" },
                { SourceLocation::MakeBuiltInFunction() }
            },
            m_integerImplementation{ integerImplementation },
            m_realImplementation{ realImplementation }
        {}

        void GetUsedSymbols(std::vector<std::string>&) const override {}
        itpr::Value Execute(itpr::Scope& scope, itpr::Stack& stack) const override;
    };

    class AstBinaryArithmeticBif : public AstFunction {
        std::vector<std::string> m_formalArgs;
        std::vector<SourceLocation> m_argLocations;
        itpr::Value(*m_integerImplementation)(long, long);
        itpr::Value(*m_realImplementation)(double, double);

    public:
        AstBinaryArithmeticBif(
            itpr::Value(integerImplementation)(long, long),
            itpr::Value(realImplementation)(double, double)) :
            AstFunction{
                SourceLocation::MakeBuiltInFunction(),
                    { "lhs", "rhs" },
                    { SourceLocation::MakeBuiltInFunction(),
                      SourceLocation::MakeBuiltInFunction() }
            },
            m_integerImplementation{ integerImplementation },
            m_realImplementation{ realImplementation }
        {}

        void GetUsedSymbols(std::vector<std::string>&) const {}
        itpr::Value Execute(itpr::Scope& scope, itpr::Stack& stack) const override;
    };

    std::vector<std::pair<std::string, std::unique_ptr<AstNode>>> BuildBifMap();

}
}
}

#endif
