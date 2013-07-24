#include <cassert>

#include "expect.h"

namespace script {

void test_expect() {

        // Test atoms.
        string str = "arbitrary string";

        // Checks: simple match.
        dom_node aster_node { dom_node_t::atom, "*", {} };
        dom_node string_node { dom_node_t::atom, str, {} };
        expect_atom exp_str { str };
        assert(check(exp_str, string_node));
        assert(!check(exp_str, aster_node));

        // Checks: capture match.
        string captured;
        dom_node list_node { dom_node_t::list, {}, { aster_node, string_node } };
        expect_atom_cap exp_cap { captured };

        assert(!check(exp_cap, list_node));
        assert(check(exp_cap, string_node));
        assert(captured == str);

        // Checks: complex match.
        expect_atom exp_ast { "*" };
        expect_atom exp_any {{}};

        expect_list<expect_atom, expect_atom> exp_list1 {
               std::make_tuple(exp_ast, exp_str)
        };
        
        expect_list<expect_atom, expect_atom> exp_list2 {
                std::make_tuple(exp_any, exp_any)
        };

        assert(check(exp_list1, list_node));
        assert(check(exp_list2, list_node));
}

}

