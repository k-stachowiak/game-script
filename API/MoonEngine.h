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
		std::vector<std::shared_ptr<ast::CAstFunction>> m_functions;
		std::map<std::string, std::unique_ptr<itpr::CGlobalScope>> m_units;

		static std::string m_DropExtension(const std::string& fileName);
		static std::string m_ReadStream(std::istream& input);
		static std::string m_ReadFile(const std::string& fileName);

		void m_InjectMapToScope(
			std::map<std::string, std::unique_ptr<ast::CAstNode>>&& map,
			itpr::CStack& stack, itpr::CScope& scope);

		std::unique_ptr<itpr::CGlobalScope> m_BuildUnitScope(const std::string& source);
		itpr::CScope* m_GetUnit(const std::string& unitName) const;

	public:
		CEngine();

		void LoadUnitFile(const std::string& fileName);
		void LoadUnitStream(const std::string& unitName, std::istream& input);
		void LoadUnitString(const std::string& unitName, const std::string& source);

		std::vector<std::string> GetAllValues(const std::string& unitName) const;
		std::vector<std::string> GetAllFunctions(const std::string& unitName) const;

		CValue GetValue(const std::string& unitName, const std::string& symbol) const;
		CValue CallFunction(const std::string& unitName, const std::string& symbol, const std::vector<CValue>& args) const;
	};

}

#endif
