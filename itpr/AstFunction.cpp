#include "AstFunction.h"
#include "Scope.h"

namespace moon {
namespace itpr {

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

CValue CAstFunction::Evaluate(CScope& scope, CStack&) const
{
	std::vector<std::string> symbols;
	GetUsedSymbols(symbols);

	std::vector<std::string> names;
	std::vector<CValue> values;
	std::vector<CSourceLocation> locations;
	scope.FindNonGlobalValues(symbols, names, values, locations);

	return CValue::MakeFunction(this, names, values, locations, {});
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
