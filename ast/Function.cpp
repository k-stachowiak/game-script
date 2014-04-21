#include "Function.h"
#include "../itpr/Scope.h"

namespace moon {
namespace ast {

CAstFunction::CAstFunction(
	const CSourceLocation& location,
	const std::vector<std::string>& formalArgs,
	const std::vector<CSourceLocation>& argLocations) :
		CAstNode{ location },
		m_formalArgs{ formalArgs },
		m_argLocations{ argLocations }
{
	assert(m_formalArgs.size() == m_argLocations.size());
}

CValue CAstFunction::Evaluate(itpr::CScope& scope, itpr::CStack&) const
{
	std::vector<std::string> symbols;
	GetUsedSymbols(symbols);
	auto captures = scope.CaptureNonGlobals(symbols);
	return CValue::MakeFunction(this, captures, {});
}

const std::vector<std::string>& CAstFunction::GetFormalArgs() const
{
	return m_formalArgs;
}

const std::vector<CSourceLocation>& CAstFunction::GetArgLocations() const
{
	return m_argLocations;
}

int CAstFunction::GetArgsCount() const
{
	return m_formalArgs.size();
}

}
}
