#include "dom_expect.h"

#include <unittest++/UnitTest++.h>

namespace
{

    const std::string ARBITRARY_ATOM1 = "arbitrary-atom1";
    const std::string ARBITRARY_ATOM2 = "arbitrary-atom2";

}

SUITE(DomExpectedTestSuite)
{

    TEST(SimpleMatch)
    {
        script::node string_node1 { script::node_type::atom, ARBITRARY_ATOM1, {} };
        script::node string_node2 { script::node_type::atom, ARBITRARY_ATOM2, {} };

        script::expect_atom exp_str { ARBITRARY_ATOM1 };

        CHECK(check(exp_str, string_node1));
        CHECK(!check(exp_str, string_node2));
    }

    TEST(CaptureMatch)
    {

        script::node string_node { script::node_type::atom, ARBITRARY_ATOM1, {} };

        script::node list_node {
            script::node_type::list,
            {},
            {
                {
                    script::node_type::atom,
                    ARBITRARY_ATOM1,
                    {}
                },
                {
                    script::node_type::atom,
                    ARBITRARY_ATOM2,
                    {}
                }
            }
        };

        std::string captured;
        script::expect_atom_cap exp_cap { captured };

        CHECK(!check(exp_cap, list_node));
        CHECK(check(exp_cap, string_node));
        CHECK(captured == ARBITRARY_ATOM1);
    }

    TEST(ComplexMatch)
    {

        script::node list_node {
            script::node_type::list,
            {},
            {
                {
                    script::node_type::atom,
                    ARBITRARY_ATOM1,
                    {}
                },
                {
                    script::node_type::atom,
                    ARBITRARY_ATOM2,
                    {}
                }
            }
        };

        script::expect_atom exp_ast { ARBITRARY_ATOM1 };
        script::expect_atom exp_any {{}};

        script::expect_list<script::expect_atom, script::expect_atom> exp_list1 {
           std::make_tuple(exp_ast, exp_any)
        };
        
        script::expect_list<script::expect_atom, script::expect_atom> exp_list2 {
            std::make_tuple(exp_any, exp_any)
        };

        CHECK(check(exp_list1, list_node));
        CHECK(check(exp_list2, list_node));
    }

}

int main()
{
    return UnitTest::RunAllTests();
}
