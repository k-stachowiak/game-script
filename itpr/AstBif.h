#ifndef MOON_ITPR_BIF_AST_BIF_H
#define MOON_ITPR_BIF_AST_BIF_H

#include <map>
#include <array>
#include <string>
#include <memory>
#include <functional>

#include "AstFunction.h"

namespace moon {
namespace itpr {
namespace bif {

	// Note:
	// The "arithmetic" attribute means that there is some
	// argument type conversion magic happening if the argument
	// type doesn't match the ewquired one but is also numeric.

	class CAstUnaryArithmeticBif : public CAstFunction {
		std::vector<std::string> m_formalArgs;
		std::vector<CSourceLocation> m_argLocations;
		CValue(*m_integerImplementation)(long);
		CValue(*m_realImplementation)(double);

	public:
		CAstUnaryArithmeticBif(
			CValue(integerImplementation)(long),
			CValue(realImplementation)(double)) :
			CAstFunction{ CSourceLocation::MakeBuiltInFunction() },
			m_integerImplementation{ integerImplementation },
			m_realImplementation{ realImplementation }
		{
			m_formalArgs.push_back("x");
			m_argLocations.push_back(CSourceLocation::MakeBuiltInFunction());
		}

		CValue Execute(CScope& scope, CStack& stack) const override;

		const std::vector<std::string>& GetFormalArgs() const override
		{
			return m_formalArgs;
		}

		const std::vector<CSourceLocation>& GetArgLocations() const override
		{
			return m_argLocations;
		}

		int GetArgsCount() const override
		{
			return m_formalArgs.size();
		}
	};

	class CAstBinaryArithmeticBif : public CAstFunction {
		std::vector<std::string> m_formalArgs;
		std::vector<CSourceLocation> m_argLocations;
		CValue(*m_integerImplementation)(long, long);
		CValue(*m_realImplementation)(double, double);

	public:
		CAstBinaryArithmeticBif(
			CValue(integerImplementation)(long, long),
			CValue(realImplementation)(double, double)) :
			CAstFunction{ CSourceLocation::MakeBuiltInFunction() },
			m_integerImplementation{ integerImplementation },
			m_realImplementation{ realImplementation }
		{
			m_formalArgs.push_back("lhs");
			m_formalArgs.push_back("rhs");
			m_argLocations.push_back(CSourceLocation::MakeBuiltInFunction());
			m_argLocations.push_back(CSourceLocation::MakeBuiltInFunction());
		}

		CValue Execute(CScope& scope, CStack& stack) const override;

		const std::vector<std::string>& GetFormalArgs() const
		{
			return m_formalArgs;
		}

		const std::vector<CSourceLocation>& GetArgLocations() const
		{
			return m_argLocations;
		}
		
		int GetArgsCount() const override
		{
			return m_formalArgs.size();
		}
	};

	std::map<std::string, std::unique_ptr<CAstNode>> BuildBifMap();

}
}
}

#endif
