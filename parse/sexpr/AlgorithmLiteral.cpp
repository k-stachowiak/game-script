#include <algorithm>
#include <sstream>

#include "Exceptions.h"
#include "Algorithm.h"

namespace moon {
namespace parse {
namespace sexpr {

    bool ParseLiteralBoolean(const std::string& atom, itpr::CValue& result)
    {
        if (atom == "true") {
            result = itpr::CValue::MakeBoolean(1);
            return true;
        } else if (atom == "false") {
            result = itpr::CValue::MakeBoolean(0);
            return true;
        } else {
            return false;
        }
    }

    bool ParseLiteralString(
        const std::string& atom, 
        const CSourceLocation& location,
        itpr::CValue& result)
    {
        char delim = TOK_STR_DELIM;

        if ((atom.front() == delim && atom.back() != delim) ||
            (atom.front() != delim && atom.back() == delim)) {
            throw ExMalformedDelimitedLiteral{ location };
        }

        unsigned length = std::distance(begin(atom), end(atom));
        if (atom.front() == delim && atom.back() == delim && length >= 2) {
            result = itpr::CValue::MakeString(atom.substr(1, length - 2));
            return true;
        } else {
            return false;
        }
    }

    bool ParseLiteralCharacter(
        const std::string& atom,
        const CSourceLocation& location,
        itpr::CValue& result)
    {
        char delim = TOK_CHAR_DELIM;

        if ((atom.front() == delim && atom.back() != delim) ||
            (atom.front() != delim && atom.back() == delim)) {
            throw ExMalformedDelimitedLiteral{ location };
        }

        unsigned length = std::distance(begin(atom), end(atom));
        if (atom.front() == delim && atom.back() == delim) {
            if ((length > 4) || (length < 3) || (length == 4 && atom.at(1) != '\\')) {
                throw ExMalformedDelimitedLiteral{ location };
            }
            result = itpr::CValue::MakeCharacter(atom.at(length - 2));
            return true;
        }
        else {
            return false;
        }
    }

    bool ParseLiteralInteger(const std::string& atom, itpr::CValue& result)
    {
        auto it = begin(atom);
        if (!atom.empty() && (isdigit(*it) || (*it) == '-' || (*it) == '+') &&
            std::all_of(it + 1, end(atom), isdigit)) {
            std::stringstream ss;
            ss << atom;
            long integer;
            ss >> integer;
            result = itpr::CValue::MakeInteger(integer);
            return true;
        } else {
            return false;
        }
    }

    bool ParseLiteralReal(const std::string& atom, itpr::CValue& result)
    {
        std::stringstream ss;
        ss << atom;
        double real;
        ss >> real;
        if (ss.eof()) {
            result = itpr::CValue::MakeReal(real);
            return true;
        } else {
            return false;
        }
    }

}
}
}
