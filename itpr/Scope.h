#ifndef MOON_ITPR_SCOPE_H
#define MOON_ITPR_SCOPE_H

#include <memory>
#include <string>
#include <map>

#include "AstNode.h"
#include "Value.h"

namespace moon {
namespace itpr {

	class CScope
	{
		std::map<std::string, itpr::CValue> m_binds;
		std::map<std::string, std::unique_ptr<itpr::CAstNode>> m_funcDecls;

	public:
		void RegisterValue(const std::string& name, itpr::CValue value);
		void RegisterFunction(const std::string& name, std::unique_ptr<itpr::CAstNode>&& expr);
		itpr::CValue GetBind(const std::string& name);
		itpr::CAstNode& GetFunction(const std::string& name);
	};

}
}

#endif