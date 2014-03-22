#include <stdexcept>
#include <fstream>
#include <sstream>
#include <memory>

#include "MoonEngine.h"
#include "../itpr/AstFuncDecl.h"
#include "../itpr/AstBind.h"
#include "../itpr/Scope.h"

namespace moon {

	std::string CMoonEngine::m_DropExtension(const std::string& fileName)
	{
		auto lastDot = fileName.rfind('.');
		return fileName.substr(0, lastDot);
	}

	std::string CMoonEngine::m_ReadFile(const std::string& fileName)
	{
		std::ifstream file{ fileName.c_str() };
		if (!file.is_open()) {
			throw std::invalid_argument{ "File not found." };
		}

		std::string line;
		std::stringstream resultStream;
		while (std::getline(file, line)) {
			resultStream << line << std::endl;
		}

		return resultStream.str();
	}

	const std::unique_ptr<itpr::CScope>& CMoonEngine::m_GetUnit(const std::string& unitName)
	{
		if (m_units.find(unitName) == end(m_units)) {
			throw std::invalid_argument{ "Invalid unit requested." };
		}

		return m_units[unitName];
	}

	void CMoonEngine::LoadUnit(const std::string& fileName)
	{
		std::string unitName = m_DropExtension(fileName);

		if (m_units.find(unitName) != end(m_units)) {
			throw std::invalid_argument{ "Already contains unit of this name" };
		}

		std::string source = m_ReadFile(fileName);
		std::unique_ptr<itpr::CScope> unit = m_parser->Parse(source);		
		m_units[unitName] = std::move(unit);
	}

	CValue CMoonEngine::GetValue(const std::string& unitName, const std::string& name)
	{
		const auto& unitScope = m_GetUnit(unitName);
		return unitScope->GetBind(name)->Evaluate(*unitScope);
	}

	CValue CMoonEngine::CallFunction(
		const std::string& unitName,
		const std::string& symbol,
		const std::vector<CValue>& args)
	{
		const auto& unitScope = m_GetUnit(unitName);
		return itpr::CallFunction(*unitScope, symbol, args);
	}

}
