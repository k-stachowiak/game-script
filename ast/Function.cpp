#include "Function.h"
#include "../itpr/Scope.h"

namespace moon {
namespace ast {

AstFunction::AstFunction(
    const SourceLocation& location,
    const std::vector<std::string>& formalArgs,
    const std::vector<SourceLocation>& argLocations) :
        AstNode{ location },
        m_formalArgs{ formalArgs },
        m_argLocations{ argLocations }
{
    assert(m_formalArgs.size() == m_argLocations.size());
}

itpr::Value AstFunction::Evaluate(itpr::Scope& scope, itpr::Stack&) const
{
    std::vector<std::string> symbols;
    GetUsedSymbols(symbols);
    auto captures = scope.CaptureNonGlobals(symbols);
    return itpr::Value::MakeFunction(this, captures, {});
}

const std::vector<std::string>& AstFunction::GetFormalArgs() const
{
    return m_formalArgs;
}

const std::vector<SourceLocation>& AstFunction::GetArgLocations() const
{
    return m_argLocations;
}

int AstFunction::GetArgsCount() const
{
    return m_formalArgs.size();
}

}
}
