#ifndef TOK_H
#define TOK_H

#include <string>
#include <vector>

namespace script
{

    enum class token_char : char
    {
        lopen = '(',    // list opening token
        lclose = ')',   // list closing token
        comment = ';',  // comment initializing token
    };

    std::vector<std::string> tokenize(const std::string&);

}

#endif
