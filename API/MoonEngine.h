#ifndef MOON_API_MOON_ENGINE_H
#define MOON_API_MOON_ENGINE_H

#include <vector>
#include <string>
#include <memory>
#include <map>

#include "../itpr/Scope.h"
#include "../parse/Parser.h"

namespace moon {

	class CMoonEngine
	{
		std::unique_ptr<parse::CParser> m_parser;
		std::map<std::string, std::unique_ptr<itpr::CScope>> m_units;

		static std::string m_DropExtension(const std::string& fileName);
		static std::string m_ReadFile(const std::string& fileName);
		const std::unique_ptr<itpr::CScope>& m_GetUnit(const std::string& unitName);

	public:
		void LoadUnit(const std::string& fileName);

		itpr::CValue GetValue(const std::string& unitName, const std::string& symbol);
		itpr::CValue CallFunction(
			const std::string& unitName,
			const std::string& symbol,
			const std::vector<itpr::CValue>& args);
	};

}

#endif
