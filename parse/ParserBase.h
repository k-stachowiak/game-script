#ifndef MOON_PARSE_PARSER_BASE_H
#define MOON_PARSE_PARSER_BASE_H

#include <string>
#include <map>
#include <memory>

#include "../ast/Bind.h"

namespace moon {
namespace parse {

    class ParserBase {
    protected:
        static std::pair<std::string, std::unique_ptr<ast::AstNode>> t_StripBind(
                std::unique_ptr<ast::AstBind>&& bind)
        {
            return std::make_pair(bind->GetSymbol(), bind->TakeOverExpression());
        }

    public:
        virtual ~ParserBase() {}

        virtual std::vector<std::pair<std::string, std::unique_ptr<ast::AstNode>>>
        Parse(const std::string& source) const = 0;
    };

}
}

#endif
