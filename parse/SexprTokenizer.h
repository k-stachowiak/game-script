#ifndef MOON_PARSE_SEXPR_TOKENIZER_H
#define MOON_PARSE_SEXPR_TOKENIZER_H

#include <vector>

#include "SexprToken.h"
#include "StrIter.h"

namespace moon {
namespace parse {

	class CSexprTokenizer {
	public:
		static std::vector<CSexprToken> Tokenize(CStrIter current, const CStrIter& last);
	};

}
}


#endif