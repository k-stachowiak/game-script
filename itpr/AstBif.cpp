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

	CValue CAstUnaryArithmeticBif::Execute(CScope& scope, CStack& stack) const
	{
		const CAstBind* bind = scope.GetBind("x");
		const CAstNode* expr = bind->TryGettingNonFunction();

		if (!expr) {
			throw ExAstReferenceToFunctionEvaluated{
				CSourceLocation::MakeBuiltInFunction(),
				stack
			};
		}

		CValue actualArgument = expr->Evaluate(scope, stack);

		if (IsFunction(actualArgument)) {
			throw ExAstReferenceToFunctionEvaluated{
				CSourceLocation::MakeBuiltInFunction(),
				stack
			};
		}

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

	CValue CAstBinaryArithmeticBif::Execute(CScope& scope, CStack& stack) const
	{
		const CAstBind* lhsBind = scope.GetBind("lhs");
		const CAstBind* rhsBind = scope.GetBind("rhs");

		const CAstNode* lhsExpr = lhsBind->TryGettingNonFunction();
		const CAstNode* rhsExpr = rhsBind->TryGettingNonFunction();

		if (!lhsBind || !rhsBind) {
			throw ExAstReferenceToFunctionEvaluated{
				CSourceLocation::MakeBuiltInFunction(),
				stack
			};
		}

		CValue lhs = lhsExpr->Evaluate(scope, stack);
		CValue rhs = rhsExpr->Evaluate(scope, stack);

		if (IsFunction(lhs) || IsFunction(rhs)) {
			throw ExAstReferenceToFunctionEvaluated{
				CSourceLocation::MakeBuiltInFunction(),
				stack
			};
		}

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
