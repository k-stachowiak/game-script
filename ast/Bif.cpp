#include <cmath>

#include "../API/Value.h"
#include "../itpr/Exceptions.h"
#include "../itpr/Scope.h"
#include "Bind.h"
#include "Bif.h"

namespace moon {
namespace ast {
namespace bif {

	// Build-in functions implementation.
	// ==================================

	CValue AddInteger(long lhs, long rhs) { return CValue::MakeInteger(lhs + rhs); }
	CValue SubInteger(long lhs, long rhs) { return CValue::MakeInteger(lhs - rhs); }
	CValue MulInteger(long lhs, long rhs) { return CValue::MakeInteger(lhs * rhs); }
	CValue DivInteger(long lhs, long rhs) { return CValue::MakeInteger(lhs / rhs); }

	CValue AddReal(double lhs, double rhs) { return CValue::MakeReal(lhs + rhs); }
	CValue SubReal(double lhs, double rhs) { return CValue::MakeReal(lhs - rhs); }
	CValue MulReal(double lhs, double rhs) { return CValue::MakeReal(lhs * rhs); }
	CValue DivReal(double lhs, double rhs) { return CValue::MakeReal(lhs / rhs); }
	
	CValue SqrtInteger(long x) { return CValue::MakeInteger(static_cast<long>(sqrt(x))); }
	CValue SqrtReal(double x)  { return CValue::MakeReal(sqrt(x)); }

	// AST part implementation.
	// ========================

	CValue CAstUnaryArithmeticBif::Execute(itpr::CScope& scope, itpr::CStack& stack) const
	{
		CValue actualArgument = scope.GetValue("x", GetLocation(), stack);

		switch (actualArgument.GetType()) {
		case EValueType::INTEGER:
			return m_integerImplementation(actualArgument.GetInteger());

		case EValueType::REAL:
			return m_realImplementation(actualArgument.GetReal());

		default:
			throw itpr::ExAstArithmeticTypeMismatch{
				CSourceLocation::MakeBuiltInFunction(),
				stack
			};
		}
	}

	CValue CAstBinaryArithmeticBif::Execute(itpr::CScope& scope, itpr::CStack& stack) const
	{
		CValue rhs = scope.GetValue("rhs", GetLocation(), stack);
		CValue lhs = scope.GetValue("lhs", GetLocation(), stack);

		if (IsInteger(lhs) && IsInteger(rhs)) {
			return m_integerImplementation(lhs.GetInteger(), rhs.GetInteger());

		} else if (IsInteger(lhs) && IsReal(rhs)) {
			return m_realImplementation(static_cast<double>(lhs.GetInteger()), rhs.GetReal());

		} else if (IsReal(lhs) && IsInteger(rhs)) {
			return m_realImplementation(lhs.GetReal(), static_cast<double>(rhs.GetInteger()));

		} else if (IsReal(lhs) && IsReal(rhs)) {
			return m_realImplementation(lhs.GetReal(), rhs.GetReal());

		} else {
			throw itpr::ExAstArithmeticTypeMismatch{
				CSourceLocation::MakeBuiltInFunction(),
				stack
			};
		}		
	}

	std::map<std::string, std::unique_ptr<CAstNode>> BuildBifMap()
	{
		std::map<std::string, std::unique_ptr<CAstNode>> result;

		result["+"] = std::unique_ptr<CAstNode> { new CAstBinaryArithmeticBif{ AddInteger, AddReal } };
		result["-"] = std::unique_ptr<CAstNode> { new CAstBinaryArithmeticBif{ SubInteger, SubReal } };
		result["*"] = std::unique_ptr<CAstNode> { new CAstBinaryArithmeticBif{ MulInteger, MulReal } };
		result["/"] = std::unique_ptr<CAstNode> { new CAstBinaryArithmeticBif{ DivInteger, DivReal } };

		result["sqrt"] = std::unique_ptr<CAstNode> { new CAstUnaryArithmeticBif{ SqrtInteger, SqrtReal } };

		return result;
	}

}
}
}
