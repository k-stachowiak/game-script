#include "dom.h"

#include <unittest++/UnitTest++.h>

namespace
{
    const std::string ARBITRARY_ATOM_STRING = "arbitrary_atom_string";
}

SUITE(DomBuilderTestSuite)
{
    TEST(EmptyList)
    {
        std::vector<std::string> tokens {};
        script::node actual_root = script::build_dom_tree(tokens);
        script::node expected_root { script::node_type::list, {}, {} };
        CHECK(expected_root == actual_root);
    }

    TEST(SingleAtomList)
    {
        std::vector<std::string> tokens { ARBITRARY_ATOM_STRING };

        script::node actual_root = script::build_dom_tree(tokens);

        script::node expected_root {
            script::node_type::list,
            {},
            {{
                script::node_type::atom,
                ARBITRARY_ATOM_STRING,
                {}
            }}
        };

        CHECK(expected_root == actual_root);
    }

    TEST(SubList)
    {
        std::vector<std::string> tokens
        {
            ARBITRARY_ATOM_STRING,
            "(",
            ARBITRARY_ATOM_STRING,
            ARBITRARY_ATOM_STRING,
            ")"
        };

        script::node actual_root = script::build_dom_tree(tokens);

        script::node expected_root {
            script::node_type::list,
            {},
            {
                {
                    script::node_type::atom,
                    ARBITRARY_ATOM_STRING,
                    {}
                },
                {
                    script::node_type::list,
                    {},
                    {
                        {
                            script::node_type::atom,
                            ARBITRARY_ATOM_STRING,
                            {}
                        },
                        {
                            script::node_type::atom,
                            ARBITRARY_ATOM_STRING,
                            {}
                        },
                    }
                }
            }
        };

        CHECK(expected_root == actual_root);
    }
}

int main()
{
    return UnitTest::RunAllTests();
}

