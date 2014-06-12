#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>

#include "Exceptions.h"
#include "Interpreter.h"
#include "../parse/ParserBase.h"
#include "../itpr/Algorithm.h"
#include "../ast/FuncDef.h"
#include "../ast/Bind.h"
#include "../ast/Bif.h"
#include "../itpr/Scope.h"
#include "../itpr/Stack.h"
#include "../parse/sexpr/AstParser.h"

namespace moon {
namespace itpr {

    std::string Interpreter::m_DropExtension(const std::string& fileName)
    {
        auto lastDot = fileName.rfind('.');
        return fileName.substr(0, lastDot);
    }

    std::string Interpreter::m_ReadStream(std::istream& input)
    {
        std::string line;
        std::stringstream resultStream;
        while (std::getline(input, line)) {
            resultStream << line << std::endl;
        }

        return resultStream.str();
    }

    std::string Interpreter::m_ReadFile(const std::string& fileName)
    {
        std::ifstream fileStream{ fileName.c_str() };
        if (!fileStream.is_open()) {
            throw ExFileNotFound{ fileName };
        }

        std::string result = m_ReadStream(fileStream);

        fileStream.close();

        return result;
    }

    void Interpreter::m_InjectMapToScope(
        std::vector<std::pair<std::string, std::unique_ptr<ast::AstNode>>>&& map,
        itpr::Stack& stack, itpr::Scope& scope)
    {
        for (auto&& pr : map) {

            Value value = pr.second->Evaluate(scope, stack);

            // Store the pointer so that the function pointers in the CValue remain valid.
            // Note that it is only possible because the map is passed in as an rvalue reference.
            // Also note that this is quite suspicious and is a candidate for refactoring.
            ast::AstFunction* maybeFunction =
                dynamic_cast<ast::AstFunction*>(pr.second.get());

            if (maybeFunction) {
                pr.second.release();
                m_functions.push_back(std::shared_ptr<ast::AstFunction>(maybeFunction));
            }

            scope.TryRegisteringBind(
                stack,
                pr.first,
                value,
                m_functions.back()->GetLocation());
        }
    }

    std::unique_ptr<itpr::GlobalScope> Interpreter::m_BuildUnitScope(const std::string& source)
    {
        auto unit = std::unique_ptr<itpr::GlobalScope> { new itpr::GlobalScope() };

        itpr::Stack stack;
        m_InjectMapToScope(ast::bif::BuildBifMap(), stack, *unit);
        m_InjectMapToScope(m_parser->Parse(source), stack, *unit);

        return unit;
    }

    itpr::Scope* Interpreter::m_GetUnit(const std::string& unitName) const
    {
        if (m_units.find(unitName) == end(m_units)) {
            throw ExUnitNotRegistered{ unitName };
        }

        return m_units.at(unitName).get();
    }

    Interpreter::Interpreter()
    : m_parser{ new parse::sexpr::AstParser }
    {}

    void Interpreter::LoadUnitFile(const std::string& fileName)
    {
        std::string unitName = m_DropExtension(fileName);

        if (m_units.find(unitName) != end(m_units)) {
            throw ExUnitAlreadyRegistered{ unitName };
        }

        std::string source = m_ReadFile(fileName);

        m_units[unitName] = m_BuildUnitScope(source);
    }

    void Interpreter::LoadUnitStream(const std::string& unitName, std::istream& input)
    {
        if (m_units.find(unitName) != end(m_units)) {
            throw ExUnitAlreadyRegistered{ unitName };
        }

        std::string source = m_ReadStream(input);

        m_units[unitName] = m_BuildUnitScope(source);
    }

    void Interpreter::LoadUnitString(const std::string& unitName, const std::string& source)
    {
        if (m_units.find(unitName) != end(m_units)) {
            throw ExUnitAlreadyRegistered{ unitName };
        }

        m_units[unitName] = m_BuildUnitScope(source);
    }


    std::vector<std::string> Interpreter::GetAllValues(const std::string& unitName) const
    {
        const auto* unitScope = m_GetUnit(unitName);
        return unitScope->GetAllValues();
    }

    std::vector<std::string> Interpreter::GetAllFunctions(const std::string& unitName) const
    {
        const auto* unitScope = m_GetUnit(unitName);
        return unitScope->GetAllFunctions();
    }

    Value Interpreter::GetValue(const std::string& unitName, const std::string& name) const
    {
        auto* unitScope = m_GetUnit(unitName);
        itpr::Stack stack;
        Value result = unitScope->GetValue(name, SourceLocation::MakeExternalInvoke(), stack);
        if (IsFunction(result)) {
            throw ExValueRequestedFromFuncBind{};
        } else {
            return result;
        }
    }

    Value Interpreter::CallFunction(
        const std::string& unitName,
        const std::string& symbol,
        const std::vector<Value>& args) const
    {
        auto* unitScope = m_GetUnit(unitName);
        itpr::Stack stack;
        return itpr::CallFunction(
            *unitScope,
            stack,
            SourceLocation::MakeExternalInvoke(),
            symbol,
            args);
    }

}
}
