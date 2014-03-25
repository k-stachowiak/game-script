#include <iterator>

#include "../../except/DomBuilder.h"
#include "Algorithm.h"

namespace moon {
namespace parse {
namespace sexpr {

	static EDomCompoundType InferCompoundType(const CToken& token)
	{
		if (token == TOK_CORE_OPEN || token == TOK_CORE_CLOSE) {
			return EDomCompoundType::CPD_CORE;
		}

		if (token == TOK_ARR_OPEN || token == TOK_ARR_CLOSE) {
			return EDomCompoundType::CPD_ARRAY;
		}

		if (token == TOK_TUP_OPEN || token == TOK_TUP_CLOSE) {
			return EDomCompoundType::CPD_TUPLE;
		}

		throw except::ExDomBuilder::UnexpectedCompoundDelimiter{};
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
					begin->GetLine(),
					begin->GetColumn(),
					InferCompoundType(*begin),
					children);
				return ++current;
			} else {
				current = TryParseAnyDomNode(current, last, inserter);
			}
		}

		throw except::ExDomBuilder::UnclosedCompoundNode{};
	}

	template <class In, class Out>
	static inline In TryParseAtomDomNode(In current, In last, Out out)
	{
		(void)last;
		*(out++) = CDomNode::MakeAtom(
			current->GetLine(),
			current->GetColumn(),
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
