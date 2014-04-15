#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>

#include "Exceptions.h"
#include "MoonEngine.h"
#include "../parse/ParserBase.h"
#include "../itpr/AstFuncDef.h"
#include "../itpr/AstBind.h"
#include "../itpr/Scope.h"
#include "../itpr/Stack.h"
#include "../itpr/AstBif.h"
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
		std::map<std::string, std::unique_ptr<itpr::CAstNode>>&& map,
		itpr::CScope& scope)
	{
		for (auto&& pr : map) {
			scope.RegisterBind(
				pr.second->GetLocation(),
				pr.first,
				std::move(pr.second));
		}
	}

	std::unique_ptr<itpr::CScope> CEngine::m_BuildUnitScope(const std::string& source)
	{
		auto unit = std::unique_ptr<itpr::CScope> { new itpr::CScope() };

		m_InjectMapToScope(itpr::bif::BuildBifMap(), *unit);
		m_InjectMapToScope(m_parser->Parse(source), *unit);

		return unit;
	}

	itpr::CScope* CEngine::m_GetUnit(const std::string& unitName)
	{
		if (m_units.find(unitName) == end(m_units)) {
			throw ExUnitNotRegistered{ unitName };
		}

		return m_units[unitName].get();
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

	CValue CEngine::GetValue(const std::string& unitName, const std::string& name)
	{
		auto* unitScope = m_GetUnit(unitName);
		const auto* bind = unitScope->GetBind(name);
		const auto& expression = bind->GetExpression();

		itpr::CStack stack;
		CValue result = expression.Evaluate(*unitScope, stack);

		if (IsFunction(result)) {
			throw ExValueRequestedFromFuncBind{};
		} else {
			return result;
		}
	}

	CValue CEngine::CallFunction(
		const std::string& unitName,
		const std::string& symbol,
		const std::vector<CValue>& args)
	{
		auto* unitScope = m_GetUnit(unitName);
		itpr::CStack stack;
		return unitScope->CallFunction(
			stack,
			CSourceLocation::MakeExternalInvoke(),
			symbol,
			args);
	}

}
