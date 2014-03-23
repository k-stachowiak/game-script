#ifndef MOON_ITPR_BIF_AST_BIF_H
#define MOON_ITPR_BIF_AST_BIF_H

#include <map>
#include <array>
#include <string>
#include <memory>
#include <functional>

#include "../AstNode.h"

namespace moon {
namespace itpr {
namespace bif {

	// Note:
	// The "arithmetic" attribute means that there is some
	// argument type conversion magic happening if the argument
	// type doesn't match the ewquired one but is also numeric.

	class CAstUnaryArithmeticBif : public CAstNode {
		CValue(*m_integerImplementation)(long);
		CValue(*m_realImplementation)(double);

	public:
		CAstUnaryArithmeticBif(
			CValue(integerImplementation)(long),
			CValue(realImplementation)(double)) :
			m_integerImplementation(integerImplementation),
			m_realImplementation(realImplementation)
		{}

		CValue Evaluate(CScope& scope) const override;
	};

	class CAstBinaryArithmeticBif : public CAstNode {
		CValue(*m_integerImplementation)(long, long);
		CValue(*m_realImplementation)(double, double);

	public:
		CAstBinaryArithmeticBif(
			CValue(integerImplementation)(long, long),
			CValue(realImplementation)(double, double)) :
			m_integerImplementation(integerImplementation),
			m_realImplementation(realImplementation)
		{}

		CValue Evaluate(CScope& scope) const override;
	};

	std::map<std::string, std::unique_ptr<CAstNode>> BuildBifMap();

}
}
}

#endif