#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>

#include "Exceptions.h"
#include "MoonEngine.h"
#include "../parse/ParserBase.h"
#include "../itpr/Algorithm.h"
#include "../ast/FuncDef.h"
#include "../ast/Bind.h"
#include "../ast/Bif.h"
#include "../itpr/Scope.h"
#include "../itpr/Stack.h"
#include "../parse/sexpr/AstParser.h"

namespace moon {

	std::string CEngine::m_DropExtension(const std::string& fileName)
	{
		auto lastDot = fileName.rfind('.');
		return fileName.substr(0, lastDot);
	}

	std::string CEngine::m_ReadStream(std::istream& input)
	{
		std::string line;
		std::stringstream resultStream;
		while (std::getline(input, line)) {
			resultStream << line << std::endl;
		}

		return resultStream.str();
	}

	std::string CEngine::m_ReadFile(const std::string& fileName)
	{
		std::ifstream fileStream{ fileName.c_str() };
		if (!fileStream.is_open()) {
			throw ExFileNotFound{ fileName };
		}

		std::string result = m_ReadStream(fileStream);

		fileStream.close();

		return result;
	}

	void CEngine::m_InjectMapToScope(
		std::map<std::string, std::unique_ptr<ast::CAstNode>>&& map,
		itpr::CStack& stack, itpr::CScope& scope)
	{
		for (auto&& pr : map) {

			CValue value = pr.second->Evaluate(scope, stack);

			// Store the pointer so that the function pointers in the CValue remain valid.
			// Note that it is only possible because the map is passed in as an rvalue reference.
			// Also note that this is quite suspicious and is a candidate for refactoring.
			ast::CAstFunction* maybeFunction =
				dynamic_cast<ast::CAstFunction*>(pr.second.get());

			if (maybeFunction) {
				pr.second.release();
				m_functions.push_back(std::shared_ptr<ast::CAstFunction>(maybeFunction));
			}

			scope.TryRegisteringBind(
				stack,
				pr.first,
				value,
				m_functions.back()->GetLocation());
		}
	}

	std::unique_ptr<itpr::CGlobalScope> CEngine::m_BuildUnitScope(const std::string& source)
	{
		auto unit = std::unique_ptr<itpr::CGlobalScope> { new itpr::CGlobalScope() };

		itpr::CStack stack;
		m_InjectMapToScope(ast::bif::BuildBifMap(), stack, *unit);
		m_InjectMapToScope(m_parser->Parse(source), stack, *unit);

		return unit;
	}

	itpr::CScope* CEngine::m_GetUnit(const std::string& unitName) const
	{
		if (m_units.find(unitName) == end(m_units)) {
			throw ExUnitNotRegistered{ unitName };
		}

		return m_units.at(unitName).get();
	}

	CEngine::CEngine()
	: m_parser{ new parse::sexpr::CAstParser }
	{}

	void CEngine::LoadUnitFile(const std::string& fileName)
	{
		std::string unitName = m_DropExtension(fileName);

		if (m_units.find(unitName) != end(m_units)) {
			throw ExUnitAlreadyRegistered{ unitName };
		}

		std::string source = m_ReadFile(fileName);

		m_units[unitName] = m_BuildUnitScope(source);
	}

	void CEngine::LoadUnitStream(const std::string& unitName, std::istream& input)
	{
		if (m_units.find(unitName) != end(m_units)) {
			throw ExUnitAlreadyRegistered{ unitName };
		}

		std::string source = m_ReadStream(input);

		m_units[unitName] = m_BuildUnitScope(source);
	}

	void CEngine::LoadUnitString(const std::string& unitName, const std::string& source)
	{
		if (m_units.find(unitName) != end(m_units)) {
			throw ExUnitAlreadyRegistered{ unitName };
		}

		m_units[unitName] = m_BuildUnitScope(source);
	}


	std::vector<std::string> CEngine::GetAllValues(const std::string& unitName) const
	{
		const auto* unitScope = m_GetUnit(unitName);
		return unitScope->GetAllValues();
	}

	std::vector<std::string> CEngine::GetAllFunctions(const std::string& unitName) const
	{
		const auto* unitScope = m_GetUnit(unitName);
		return unitScope->GetAllFunctions();
	}

	CValue CEngine::GetValue(const std::string& unitName, const std::string& name) const
	{		
		auto* unitScope = m_GetUnit(unitName);
		itpr::CStack stack;
		CValue result = unitScope->GetValue(name, CSourceLocation::MakeExternalInvoke(), stack);
		if (IsFunction(result)) {
			throw ExValueRequestedFromFuncBind{};
		} else {
			return result;
		}
	}

	CValue CEngine::CallFunction(
		const std::string& unitName,
		const std::string& symbol,
		const std::vector<CValue>& args) const
	{
		auto* unitScope = m_GetUnit(unitName);
		itpr::CStack stack;
		return itpr::CallFunction(
			*unitScope,
			stack,
			CSourceLocation::MakeExternalInvoke(),
			symbol,
			args);
	}

}
