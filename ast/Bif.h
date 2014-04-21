#ifndef MOON_AST_BIF_H
#define MOON_AST_BIF_H

#include <map>
#include <array>
#include <string>
#include <memory>
#include <functional>

#include "Function.h"

namespace moon {
namespace ast {
namespace bif {

	// Note:
	// The "arithmetic" attribute means that there is some
	// argument type conversion magic happening if the argument
	// type doesn't match the ewquired one but is also numeric.

	class CAstUnaryArithmeticBif : public CAstFunction {
		CValue(*m_integerImplementation)(long);
		CValue(*m_realImplementation)(double);

	public:
		CAstUnaryArithmeticBif(
			CValue(integerImplementation)(long),
			CValue(realImplementation)(double)) :
			CAstFunction{
				CSourceLocation::MakeBuiltInFunction(),
				{ "x" },
				{ CSourceLocation::MakeBuiltInFunction() }
			},
			m_integerImplementation{ integerImplementation },
			m_realImplementation{ realImplementation }
		{}

		void GetUsedSymbols(std::vector<std::string>&) const override {}
		CValue Execute(itpr::CScope& scope, itpr::CStack& stack) const override;
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
			CAstFunction{
				CSourceLocation::MakeBuiltInFunction(),
					{ "lhs", "rhs" },
					{ CSourceLocation::MakeBuiltInFunction(),
					  CSourceLocation::MakeBuiltInFunction() }
			},
			m_integerImplementation{ integerImplementation },
			m_realImplementation{ realImplementation }
		{}

		void GetUsedSymbols(std::vector<std::string>&) const {}
		CValue Execute(itpr::CScope& scope, itpr::CStack& stack) const override;
	};

	std::map<std::string, std::unique_ptr<CAstNode>> BuildBifMap();

}
}
}

#endif
