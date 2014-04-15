#ifndef MOON_API_MOON_ENGINE_H
#define MOON_API_MOON_ENGINE_H

#include <vector>
#include <string>
#include <memory>
#include <iosfwd>
#include <map>

#include "Value.h"
#include "../parse/ParserBase.h"
#include "../itpr/Scope.h"

namespace moon {

	class CEngine {
		std::unique_ptr<parse::CParserBase> m_parser;
		std::map<std::string, std::unique_ptr<itpr::CScope>> m_units;

		static std::string m_DropExtension(const std::string& fileName);
		static std::string m_ReadStream(std::istream& input);
		static std::string m_ReadFile(const std::string& fileName);
		static void m_InjectMapToScope(std::map<std::string, std::unique_ptr<itpr::CAstNode>>&& map, itpr::CScope& scope);

		std::unique_ptr<itpr::CScope> m_BuildUnitScope(const std::string& source);
		itpr::CScope* m_GetUnit(const std::string& unitName);

	public:
		CEngine();

		void LoadUnitFile(const std::string& fileName);
		void LoadUnitStream(const std::string& unitName, std::istream& input);
		void LoadUnitString(const std::string& unitName, const std::string& source);

		CValue GetValue(const std::string& unitName, const std::string& symbol);
		CValue CallFunction(const std::string& unitName, const std::string& symbol, const std::vector<CValue>& args);
	};

}

#endif
