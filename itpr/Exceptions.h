#ifndef MOON_ITPR_EXCEPTIONS_H
#define MOON_ITPR_EXCEPTIONS_H

#include <sstream>
#include <string>
#include <vector>

#include "../common/SourceLocation.h"
#include "../API/Exceptions.h"
#include "Stack.h"

namespace moon {
namespace itpr {

	inline std::vector<std::string> GenStackFrames(const CStack& stack)
	{
		std::vector<std::string> result;
		stack.VisitCalls([&result](int level, const CStackFrame& frame) mutable {
			std::ostringstream oss;
			oss << level << " : " << frame.GetFunction();
			result.push_back(oss.str());
		});
		return result;
	}

	struct ExAstArithmeticTypeMismatch : public ExInterpretationError {
		ExAstArithmeticTypeMismatch(const CSourceLocation& location, const CStack& stack) :
		ExInterpretationError{
				location.GetLine(),
				location.GetColumn(),
				GenStackFrames(stack),
				"Arithmetic type mismatch." }
		{}
	};

	struct ExScopeSymbolAlreadyRegistered : public ExInterpretationError {
		ExScopeSymbolAlreadyRegistered(const CSourceLocation& location, const CStack& stack) :
		ExInterpretationError{
			location.GetLine(),
			location.GetColumn(),
			GenStackFrames(stack),
			"Symbol already registered." }
		{}
	};

	struct ExScopeSymbolNotRegistered : public ExInterpretationError {
		ExScopeSymbolNotRegistered(const CSourceLocation& location, const CStack& stack) :
		ExInterpretationError{
			location.GetLine(),
			location.GetColumn(),
			GenStackFrames(stack),
			"Symbol not registered." }
		{}
	};

	struct ExScopeSymbolIsNotFunction : public ExInterpretationError {
		ExScopeSymbolIsNotFunction(const CSourceLocation& location, const CStack& stack) :
		ExInterpretationError{
			location.GetLine(),
			location.GetColumn(),
			GenStackFrames(stack),
			"Symbol is not a function." }
		{}
	};

	struct ExScopeFormalActualArgCountMismatch : public ExInterpretationError {
		ExScopeFormalActualArgCountMismatch(const CSourceLocation& location, const CStack& stack) :
		ExInterpretationError{
			location.GetLine(),
			location.GetColumn(),
			GenStackFrames(stack),
			"Formal and actual argument call mismatch." }
		{}
	};

	struct ExInconsistentTypesInArray : public ExInterpretationError {
		ExInconsistentTypesInArray(const CSourceLocation& location, const CStack& stack) :
		ExInterpretationError{
			location.GetLine(),
			location.GetColumn(),
			GenStackFrames(stack),
			"Inconsistent types in array." }
		{}
	};

}
}

#endif
