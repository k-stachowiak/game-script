#ifndef DOM_H
#define DOM_H

#include <string>
#include <vector>

namespace script
{

    enum class node_type { atom, list };

    struct node
    {
        node_type type;
        std::string atom;
        std::vector<node> list;
    };

    inline bool operator==(const node& lhs, const node& rhs)
    {
        return lhs.type == rhs.type &&
               lhs.atom == rhs.atom &&
               lhs.list.size() == rhs.list.size() &&
               std::equal(begin(lhs.list), end(lhs.list), begin(rhs.list));
    }

    // This takes a non-const argument as a phony ")" is appended at the end
    // of the tokens list for the time of the processing.
    node build_dom_tree(std::vector<std::string>&);

}

#endif
