#include <cmath>

#include "../API/Value.h"
#include "Exceptions.h"
#include "Scope.h"
#include "AstBind.h"
#include "AstBif.h"

namespace moon {
namespace itpr {
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

	CValue CAstUnaryArithmeticBif::Execute(std::shared_ptr<CScope> scope, CStack& stack) const
	{
		CValue actualArgument = scope->GetValue("x");

		switch (actualArgument.GetType()) {
		case EValueType::INTEGER:
			return m_integerImplementation(actualArgument.GetInteger());

		case EValueType::REAL:
			return m_realImplementation(actualArgument.GetReal());

		default:
			throw ExAstArithmeticTypeMismatch{
				CSourceLocation::MakeBuiltInFunction(),
				stack
			};
		}
	}

	CValue CAstBinaryArithmeticBif::Execute(std::shared_ptr<CScope> scope, CStack& stack) const
	{
		CValue lhs = scope->GetValue("lhs");
		CValue rhs = scope->GetValue("rhs");

		if (IsInteger(lhs) && IsInteger(rhs)) {
			return m_integerImplementation(lhs.GetInteger(), rhs.GetInteger());

		} else if (IsInteger(lhs) && IsReal(rhs)) {
			return m_realImplementation(static_cast<double>(lhs.GetInteger()), rhs.GetReal());

		} else if (IsReal(lhs) && IsInteger(rhs)) {
			return m_realImplementation(lhs.GetReal(), static_cast<double>(rhs.GetInteger()));

		} else if (IsReal(lhs) && IsReal(rhs)) {
			return m_realImplementation(lhs.GetReal(), rhs.GetReal());

		} else {
			throw ExAstArithmeticTypeMismatch{
				CSourceLocation::MakeBuiltInFunction(),
				stack
			};
		}		
	}

	std::map<std::string, std::shared_ptr<const CAstFunction>> BuildBifMap()
	{
		std::map<std::string, std::shared_ptr<const CAstFunction>> result;

		result["+"] = std::shared_ptr<const CAstFunction> { new CAstBinaryArithmeticBif{ AddInteger, AddReal } };
		result["-"] = std::shared_ptr<const CAstFunction> { new CAstBinaryArithmeticBif{ SubInteger, SubReal } };
		result["*"] = std::shared_ptr<const CAstFunction> { new CAstBinaryArithmeticBif{ MulInteger, MulReal } };
		result["/"] = std::shared_ptr<const CAstFunction> { new CAstBinaryArithmeticBif{ DivInteger, DivReal } };

		result["sqrt"] = std::shared_ptr<const CAstFunction> { new CAstUnaryArithmeticBif{ SqrtInteger, SqrtReal } };

		return result;
	}

}
}
}
