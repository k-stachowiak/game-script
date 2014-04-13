#include <iterator>

#include "Exceptions.h"
#include "Algorithm.h"

namespace moon {
namespace parse {
namespace sexpr {

	static EDomCompoundType InferCompoundType(CSourceLocation location, const CToken& openingToken)
	{
		if (openingToken == TOK_CORE_OPEN) {
			return EDomCompoundType::CORE;
		}

		if (openingToken == TOK_ARR_OPEN) {
			return EDomCompoundType::ARRAY;
		}

		if (openingToken == TOK_TUP_OPEN) {
			return EDomCompoundType::TUPLE;
		}

		throw ExDomUnexpectedCompoundDelimiter{ location };
	}

	template <class In, class Out>
	static inline In TryParseAnyDomNode(In current, In last, Out out);

	template <class In, class Out>
	static inline In TryParseCompoundDomNode(In current, In last, Out out)
	{
		const auto begin = current++;

		std::vector<CDomNode> children;
		auto inserter = std::back_inserter(children);

		while (current != last) {
			if (IsClosingParenthesis(*current) && ParenthesisMatch(*begin, *current)) {
				*(out++) = CDomNode::MakeCompound(
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
		*(out++) = CDomNode::MakeAtom(
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

	std::vector<CDomNode> BuildDom(const std::vector<CToken>& tokens)
	{
		auto current = begin(tokens);
		auto last = end(tokens);

		std::vector<CDomNode> result;
		auto inserter = std::back_inserter(result);

		while (current != last) {
			current = TryParseAnyDomNode(current, last, inserter);
		}

		return result;
	}

}
}
}
