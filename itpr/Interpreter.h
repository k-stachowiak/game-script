#ifndef MOON_ITPR_INTERPRETER_H
#define MOON_ITPR_INTERPRETER_H

#include <vector>
#include <string>
#include <memory>
#include <iosfwd>
#include <map>

#include "../parse/ParserBase.h"
#include "../itpr/Value.h"
#include "../itpr/Scope.h"

namespace moon {
namespace itpr {

    class Interpreter {
        std::unique_ptr<parse::ParserBase> m_parser;
        std::vector<std::shared_ptr<ast::AstFunction>> m_functions;
        std::map<std::string, std::unique_ptr<itpr::GlobalScope>> m_units;

        static std::string m_DropExtension(const std::string& fileName);
        static std::string m_ReadStream(std::istream& input);
        static std::string m_ReadFile(const std::string& fileName);

        void m_InjectMapToScope(
            std::vector<std::pair<std::string, std::unique_ptr<ast::AstNode>>>&& map,
            itpr::Stack& stack, itpr::Scope& scope);

        std::unique_ptr<itpr::GlobalScope> m_BuildUnitScope(const std::string& source);
        itpr::Scope* m_GetUnit(const std::string& unitName) const;

    public:
        Interpreter();

        void LoadUnitFile(const std::string& fileName);
        void LoadUnitStream(const std::string& unitName, std::istream& input);
        void LoadUnitString(const std::string& unitName, const std::string& source);

        std::vector<std::string> GetAllValues(const std::string& unitName) const;
        std::vector<std::string> GetAllFunctions(const std::string& unitName) const;

        Value GetValue(const std::string& unitName, const std::string& symbol) const;
        Value CallFunction(const std::string& unitName, const std::string& symbol, const std::vector<Value>& args) const;
    };

}
}

#endif
