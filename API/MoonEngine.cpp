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

	std::shared_ptr<itpr::CScope> CEngine::m_GetUnit(const std::string& unitName)
	{
		if (m_units.find(unitName) == end(m_units)) {
			throw ExUnitNotRegistered{ unitName };
		}

		return m_units[unitName];
	}

	CEngine::CEngine() :
		m_parser{ new parse::sexpr::CAstParser },
		m_stdlibScope{ new itpr::CScope }
	{
		for (auto&& pr : itpr::bif::BuildBifMap()) {
			m_stdlibScope->RegisterBind(
					pr.second->GetLocation(),
					pr.first,
					std::move(pr.second));
		}
	}

	void CEngine::LoadUnitFile(const std::string& fileName)
	{
		std::string unitName = m_DropExtension(fileName);
		if (m_units.find(unitName) != end(m_units)) {
			throw ExUnitAlreadyRegistered{ unitName };
		}
		std::string source = m_ReadFile(fileName);
		std::shared_ptr<itpr::CScope> unit = m_parser->Parse(source, m_stdlibScope);

		m_units[unitName] = unit;
	}

	void CEngine::LoadUnitStream(const std::string& unitName, std::istream& input) {
		if (m_units.find(unitName) != end(m_units)) {
			throw ExUnitAlreadyRegistered{ unitName };
		}
		std::string source = m_ReadStream(input);
		std::shared_ptr<itpr::CScope> unit = m_parser->Parse(source, m_stdlibScope);

		m_units[unitName] = unit;
	}

	void CEngine::LoadUnitString(const std::string& unitName, const std::string& source)
	{
		if (m_units.find(unitName) != end(m_units)) {
			throw ExUnitAlreadyRegistered{ unitName };
		}
		std::shared_ptr<itpr::CScope> unit = m_parser->Parse(source, m_stdlibScope);

		m_units[unitName] = unit;
	}

	CValue CEngine::GetValue(const std::string& unitName, const std::string& name)
	{
		auto unitScope = m_GetUnit(unitName);
		const auto* bind = unitScope->GetBind(name);
		const auto& expression = bind->GetExpression();

		itpr::CStack stack;
		CValue result = expression.Evaluate(unitScope, stack);

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
		auto unitScope = m_GetUnit(unitName);
		itpr::CStack stack;
		return unitScope->CallFunction(
			stack,
			CSourceLocation::MakeExternalInvoke(),
			symbol,
			args);
	}

}
