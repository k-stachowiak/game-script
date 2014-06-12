#ifndef MOON_AST_FUNCTION_H
#define MOON_AST_FUNCTION_H

#include <cassert>
#include <utility>

#include "Node.h"

namespace moon {
namespace ast {

    class AstFunction : public AstNode {
        const std::vector<std::string> m_formalArgs;
        const std::vector<SourceLocation> m_argLocations;

    public:
        AstFunction(
            const SourceLocation& location,
            const std::vector<std::string>& formalArgs,
            const std::vector<SourceLocation>& argLocations);

        itpr::Value Evaluate(itpr::Scope& scope, itpr::Stack&) const;
        const std::vector<std::string>& GetFormalArgs() const;
        const std::vector<SourceLocation>& GetArgLocations() const;
        int GetArgsCount() const;
        virtual itpr::Value Execute(itpr::Scope& scope, itpr::Stack& stack) const = 0;
    };

}
}

#endif
