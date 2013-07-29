#ifndef DOM_BUILD_H
#define DOM_BUILD_H

#include "dom.h"

namespace script
{

    // This takes a non-const argument as a phony ")" is appended at the end
    // of the tokens list for the time of the processing.
    node build_dom_tree(std::vector<std::string>&);

}

#endif
