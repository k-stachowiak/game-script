#include <iterator>

#include "SexprDomBuilder.h"

namespace moon {
namespace parse {

	ESexprDomCompoundType InferCompoundType(const CSexprToken& token)
	{
		if (token.IsCharacter(TOK_CORE_OPEN) || token.IsCharacter(TOK_CORE_CLOSE)) {
			return ESexprDomCompoundType::CPD_CORE;
		}

		if (token.IsCharacter(TOK_ARR_OPEN) || token.IsCharacter(TOK_ARR_CLOSE)) {
			return ESexprDomCompoundType::CPD_ARRAY;
		}

		if (token.IsCharacter(TOK_TUP_OPEN) || token.IsCharacter(TOK_TUP_CLOSE)) {
			return ESexprDomCompoundType::CPD_TUPLE;
		}

		if (token.IsCharacter(TOK_LIST_OPEN) || token.IsCharacter(TOK_LIST_CLOSE)) {
			return ESexprDomCompoundType::CPD_LIST;
		}

		throw std::runtime_error{ "Unexpected compound delimiting token" };
	}

	template <class In, class Out>
	static inline In TryParseAnyDomNode(In current, In last, Out out);

	template <class In, class Out>
	static inline In TryParseCompoundDomNode(In current, In last, Out out)
	{
		const auto begin = current++;

		std::vector<CSexprDomNode> children;
		auto inserter = std::back_inserter(children);

		while (current != last) {
			if (current->IsClosingParenthesis() && g_ParenthesisMatch(*begin, *current)) {
				*(out++) = CSexprDomNode::MakeCompound(
					InferCompoundType(*begin),
					children);
				return ++current;
			} else {
				current = TryParseAnyDomNode(current, last, inserter);
			}
		}

		throw std::runtime_error{ "Unclosed compound DOM node" };
	}

	template <class In, class Out>
	static inline In TryParseAtomDomNode(In current, In last, Out out)
	{
		*(out++) = CSexprDomNode::MakeAtom(current->ToString());
		return ++current;
	}

	template <class In, class Out>
	static inline In TryParseAnyDomNode(In current, In last, Out out)
	{
		if ((*current).IsOpeningParenthesis()) {
			return TryParseCompoundDomNode(current, last, out);
		} else {
			return TryParseAtomDomNode(current, last, out);
		}
	}

	std::vector<CSexprDomNode> CSexprDomBuilder::BuildDom(const std::vector<CSexprToken>& tokens)
	{
		auto current = begin(tokens);
		auto last = end(tokens);

		std::vector<CSexprDomNode> result;
		auto inserter = std::back_inserter(result);

		while (current != last) {
			current = TryParseAnyDomNode(current, last, inserter);
		}

		return result;
	}

}
}