#ifndef MOON_PARSE_SEXPR_AST_PARSER_H
#define MOON_PARSE_SEXPR_AST_PARSER_H

#include "../ParserBase.h"

namespace moon {
namespace parse {
namespace sexpr {

    class AstParser : public ParserBase
    {
    public:
        std::vector<std::pair<std::string, std::unique_ptr<ast::AstNode>>>
        Parse(const std::string& source) const override;
    };

}
}
}

#endif
