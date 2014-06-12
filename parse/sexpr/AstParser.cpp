#include "AstParser.h"

#include "../../ast/FuncDef.h"
#include "../../itpr/Stack.h"

#include "../StrIter.h"
#include "Token.h"
#include "DomNode.h"
#include "Algorithm.h"

namespace moon {
namespace parse {
namespace sexpr {

    std::vector<std::pair<std::string, std::unique_ptr<ast::AstNode>>>
    AstParser::Parse(const std::string& source) const
    {
        std::vector<Token> tokens = Tokenize(StrIter::Begin(source), StrIter::End(source));
        std::vector<DomNode> domNodes = BuildDom(tokens);
        std::vector<std::pair<std::string, std::unique_ptr<ast::AstNode>>> result;
        for (const DomNode& domNode : domNodes) {
            auto bind = ParseBind(domNode);
            auto pair = t_StripBind(std::move(bind));
            result.push_back(std::move(pair));
        }
        return result;
    }

}
}
}
