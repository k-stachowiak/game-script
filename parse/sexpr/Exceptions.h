#ifndef MOON_PARSE_SEXPR_EXCEPTIONS_H
#define MOON_PARSE_SEXPR_EXCEPTIONS_H

#include "../../common/SourceLocation.h"
#include "../../common/Exceptions.h"

namespace moon {
namespace parse {
namespace sexpr {

    struct ExTokNonDelimitedStringOrCharacter : public ExParsingError {
        ExTokNonDelimitedStringOrCharacter(const SourceLocation& location) :
        ExParsingError{
            location.GetLine(),
            location.GetColumn(),
            "Non-delimited string or character." }
        {}
    };

    struct ExDomUnexpectedCompoundDelimiter : public ExParsingError {
        ExDomUnexpectedCompoundDelimiter(const SourceLocation& location) :
        ExParsingError{
            location.GetLine(),
            location.GetColumn(),
            "Unexpected compound delimiter." }
        {}
    };
    struct ExDomUnclosedCompoundNode : public ExParsingError {
        ExDomUnclosedCompoundNode(const SourceLocation& location) :
        ExParsingError{
            location.GetLine(),
            location.GetColumn(),
            "Unclosed component node." }
        {}
    };

    struct ExAstBindParsingFailed : public ExParsingError {
        ExAstBindParsingFailed(const SourceLocation& location) :
        ExParsingError{
            location.GetLine(),
            location.GetColumn(),
            "Bind parsing failed." }
        {}
    };

    struct ExMalformedDelimitedLiteral : public ExParsingError {
        ExMalformedDelimitedLiteral(const SourceLocation& location) :
        ExParsingError{
            location.GetLine(),
            location.GetColumn(),
            "Malformed delimited literal." }
        {}
    };
}
}
}

#endif
