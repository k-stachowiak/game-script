#include "Compound.h"
#include "../itpr/Exceptions.h"

namespace moon {
namespace ast {

    AstCompound::AstCompound(
        const SourceLocation& location,
        itpr::CompoundType type, 
        std::vector<std::unique_ptr<AstNode>>&& expressions) :
        AstNode{ location },
        m_type{ type },
        m_expressions{ std::move(expressions) }
    {}

    itpr::Value AstCompound::Evaluate(itpr::Scope& scope, itpr::Stack& stack) const
    {
        std::vector<itpr::Value> values;
        for (const auto& expression : m_expressions) {
            values.push_back(expression->Evaluate(scope, stack));
        }

        if (m_type == itpr::CompoundType::ARRAY && values.size() > 1) {
            const itpr::Value& firstValue = values.front();
            for (unsigned i = 1; i < values.size(); ++i) {
                if (!itpr::Value::TypesEqual(firstValue, values[i])) {
                    throw itpr::ExInconsistentTypesInArray(GetLocation(), stack);
                }
            }
        }

        return itpr::Value::MakeCompound(m_type, values);
    }

    void AstCompound::GetUsedSymbols(std::vector<std::string>& symbols) const
    {
        for (const auto& expression : m_expressions) {
            expression->GetUsedSymbols(symbols);
        }
    }

}
}
