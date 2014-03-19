#ifndef MOON_ITPR_SCOPE_H
#define MOON_ITPR_SCOPE_H

#include <memory>
#include <string>
#include <map>

#include "../API/Value.h"

namespace moon {
namespace itpr {

	class CAstFuncDecl;

	class CScope {
		CScope* m_parent;
		std::map<std::string, CValue> m_binds;
		std::map<std::string, std::unique_ptr<itpr::CAstFuncDecl>> m_funcDecls;

	public:
		CScope();
		CScope(CScope* parent);
		void RegisterValue(const std::string& name, CValue value);
		void RegisterFunction(const std::string& name, std::unique_ptr<itpr::CAstFuncDecl>&& expr);
		CValue GetBind(const std::string& name);
		itpr::CAstFuncDecl& GetFunction(const std::string& name);
	};

	CValue g_CallFunction(
		CScope& scope,
		itpr::CAstFuncDecl& function,
		const std::vector<CValue>& args);
}
}

#endif