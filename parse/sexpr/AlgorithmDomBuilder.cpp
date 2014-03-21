#include <iterator>

#include "Algorithm.h"

namespace moon {
namespace parse {
namespace sexpr {

	static EDomCompoundType InferCompoundType(const CToken& token)
	{
		if (token.IsCharacter(TOK_CORE_OPEN) || token.IsCharacter(TOK_CORE_CLOSE)) {
			return EDomCompoundType::CPD_CORE;
		}

		if (token.IsCharacter(TOK_ARR_OPEN) || token.IsCharacter(TOK_ARR_CLOSE)) {
			return EDomCompoundType::CPD_ARRAY;
		}

		if (token.IsCharacter(TOK_TUP_OPEN) || token.IsCharacter(TOK_TUP_CLOSE)) {
			return EDomCompoundType::CPD_TUPLE;
		}

		throw std::runtime_error{ "Unexpected compound delimiting token" };
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
			if (current->IsClosingParenthesis() && g_ParenthesisMatch(*begin, *current)) {
				*(out++) = CDomNode::MakeCompound(
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
		*(out++) = CDomNode::MakeAtom(current->ToString());
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