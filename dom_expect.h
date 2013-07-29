#ifndef EXPECT_H
#define EXPECT_H

#include <tuple>
#include <utility>

#include "dom.h"

namespace script {

    namespace detail
    {

        // Tuple helpers.
        // --------------

        template<size_t I = 0, typename... Tp>
        inline typename std::enable_if<I == sizeof...(Tp), bool>::type
        check(std::tuple<Tp...> const& tup, script::node const&)
        {
            return true;
        }

        template<std::size_t I = 0, typename... Tp>
        inline typename std::enable_if<I < sizeof...(Tp), bool>::type
        check(std::tuple<Tp...> const& tup, script::node const& n)
        {
            return check(std::get<I>(tup), n.list[I]) &&
                   check<I + 1, Tp...>(tup, n);
        }

    }

    // Expect implementation.
    // ----------------------
            
    struct expect_atom
    {
        std::string expected;

        friend bool check(expect_atom const& exp, script::node const& n)
        {
            if(n.type != script::node_type::atom)
            {
                return false;
            }

            if(exp.expected.empty())
            {
                return true;
            }

            return n.atom == exp.expected;
        }
    };

    struct expect_atom_cap
    {
        std::string& reference;

        friend bool check(expect_atom_cap const& exp, script::node const& n)
        {
            if(n.type != script::node_type::atom)
            {
                return false;
            }
            
            exp.reference = n.atom;

            return true;
        }
    };


    template<class Left, class Right, class Op>
    struct expect_binop
    {
        Left left;
        Right right;

        friend bool check(
                expect_binop<Left, Right, Op> const& exp,
                script::node const& n)
        {
            Op op;
            return op(check(exp.left, n), check(exp.right, n));
        }
    };

    template<class... Args>
    struct expect_list
    {
        std::tuple<Args...> elements;
        friend bool check(
                expect_list<Args...> const& exp,
                script::node const& n)
        {
            if(n.type != script::node_type::list)
            {
                return false;
            }

            return detail::check(exp.elements, n);
        }
    };

}

#endif
