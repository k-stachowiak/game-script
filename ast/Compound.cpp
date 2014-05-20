#include "Compound.h"
#include "../itpr/Exceptions.h"

namespace moon {
namespace ast {

    CAstCompound::CAstCompound(
        const CSourceLocation& location,
        itpr::ECompoundType type, 
        std::vector<std::unique_ptr<CAstNode>>&& expressions) :
        CAstNode{ location },
        m_type{ type },
        m_expressions{ std::move(expressions) }
    {}

    itpr::CValue CAstCompound::Evaluate(itpr::CScope& scope, itpr::CStack& stack) const
    {
        std::vector<itpr::CValue> values;
        for (const auto& expression : m_expressions) {
            values.push_back(expression->Evaluate(scope, stack));
        }

        if (m_type == itpr::ECompoundType::ARRAY && values.size() > 1) {
            const itpr::CValue& firstValue = values.front();
            for (unsigned i = 1; i < values.size(); ++i) {
                if (!itpr::CValue::TypesEqual(firstValue, values[i])) {
                    throw itpr::ExInconsistentTypesInArray(GetLocation(), stack);
                }
            }
        }

        return itpr::CValue::MakeCompound(m_type, values);
    }

    void CAstCompound::GetUsedSymbols(std::vector<std::string>& symbols) const
    {
        for (const auto& expression : m_expressions) {
            expression->GetUsedSymbols(symbols);
        }
    }

}
}
