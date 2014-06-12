#include <iterator>

#include "Exceptions.h"
#include "Algorithm.h"

namespace moon {
namespace parse {
namespace sexpr {

    static DomCompoundType InferCompoundType(SourceLocation location, const Token& openingToken)
    {
        if (openingToken == TOK_CORE_OPEN) {
            return DomCompoundType::CORE;
        }

        if (openingToken == TOK_ARR_OPEN) {
            return DomCompoundType::ARRAY;
        }

        if (openingToken == TOK_TUP_OPEN) {
            return DomCompoundType::TUPLE;
        }

        throw ExDomUnexpectedCompoundDelimiter{ location };
    }

    template <class In, class Out>
    static inline In TryParseAnyDomNode(In current, In last, Out out);

    template <class In, class Out>
    static inline In TryParseCompoundDomNode(In current, In last, Out out)
    {
        const auto begin = current++;

        std::vector<DomNode> children;
        auto inserter = std::back_inserter(children);

        while (current != last) {
            if (IsClosingParenthesis(*current) && ParenthesisMatch(*begin, *current)) {
                *(out++) = DomNode::MakeCompound(
                    begin->GetLocation(),
                    InferCompoundType(begin->GetLocation(), *begin),
                    children);
                return ++current;
            } else {
                current = TryParseAnyDomNode(current, last, inserter);
            }
        }

        throw ExDomUnclosedCompoundNode{ begin->GetLocation() };
    }

    template <class In, class Out>
    static inline In TryParseAtomDomNode(In current, In last, Out out)
    {
        (void)last;
        *(out++) = DomNode::MakeAtom(
            current->GetLocation(),
            current->ToString());
        return ++current;
    }

    template <class In, class Out>
    static inline In TryParseAnyDomNode(In current, In last, Out out)
    {
        if (IsOpeningParenthesis(*current)) {
            return TryParseCompoundDomNode(current, last, out);
        } else {
            return TryParseAtomDomNode(current, last, out);
        }
    }

    std::vector<DomNode> BuildDom(const std::vector<Token>& tokens)
    {
        auto current = begin(tokens);
        auto last = end(tokens);

        std::vector<DomNode> result;
        auto inserter = std::back_inserter(result);

        while (current != last) {
            current = TryParseAnyDomNode(current, last, inserter);
        }

        return result;
    }

}
}
}
