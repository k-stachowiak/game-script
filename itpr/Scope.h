#ifndef MOON_ITPR_SCOPE_H
#define MOON_ITPR_SCOPE_H

#include <memory>
#include <string>
#include <map>

#include "../API/Value.h"

namespace moon {
namespace itpr {

	class CAstNode;
	class CAstBind;

	class CScope {
		CScope* m_parent;
		std::vector<std::unique_ptr<CAstBind>> m_binds;
		std::map<std::string, CAstBind*> m_bind_map;

	public:
		CScope();
		CScope(CScope* parent);
		void RegisterBind(std::unique_ptr<CAstBind>&& bind);
		void RegisterBind(const std::string& name, std::unique_ptr<CAstNode>&& expression);
		CAstBind* GetBind(const std::string& name);
		CValue CallFunction(const std::string& symbol, const std::vector<CValue>& args);
	};

}
}

#endif