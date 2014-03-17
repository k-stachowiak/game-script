#include <stdexcept>
#include <fstream>
#include <sstream>

#include "MoonEngine.h"
#include "../itpr/AstFuncDecl.h"

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

	itpr::CValue CMoonEngine::GetValue(const std::string& unitName, const std::string& name)
	{
		const auto& unitScope = m_GetUnit(unitName);
		return unitScope->GetBind(name);
	}

	itpr::CValue CMoonEngine::CallFunction(
		const std::string& unitName,
		const std::string& symbol,
		const std::vector<itpr::CValue>& args)
	{
		const auto& unitScope = m_GetUnit(unitName);

		// Note: Never do this!
		itpr::CAstFuncDecl& funcDecl = dynamic_cast<itpr::CAstFuncDecl&>(unitScope->GetFunction(symbol));
		std::vector<std::string> formalArgs = funcDecl.GetFormalArgs();

		if (formalArgs.size() != args.size()) {
			throw std::invalid_argument{ "Formal/actual argument count mismatch" };
		}

		std::shared_ptr<itpr::CScope> funcScope = std::make_shared<itpr::CScope>(unitScope);

		unsigned commonSize = args.size();
		for (unsigned i = 0; i < commonSize; ++i) {
			funcScope->RegisterValue(formalArgs[i], args[i]);
		}

		return funcDecl.Evaluate(*funcScope);
	}

}
