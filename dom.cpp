/* Copyright (C) 2012,2013 Krzysztof Stachowiak */

/*
 * This file is part of space-shooter.
 *
 * space-shooter is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * space-shooter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with space-shooter; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "dom.h"

using namespace std;

namespace script {

        std::ostream& operator<<(std::ostream& out, dom_node const& n) {
                switch(n.type) {
                case dom_node_t::atom:
                        out << n.atom;
                        break;

                case dom_node_t::list:
                        out << "{ ";
                        for(dom_node const& child : n.list)
                                out << child << " ";
                        out << "}";
                        break;
                }
                return out;
        }


        static bool read_maybe_node(tokenizer&, dom_node&);

        static dom_node read_list_node(tokenizer& tok) {
                vector<dom_node> elements;
                dom_node element;
                while(true) {
                        if(!read_maybe_node(tok, element)) break;
                        elements.push_back(element);
                }
                return { dom_node_t::list, {}, elements };
        }

        static bool read_maybe_node(tokenizer& tok, dom_node& result) {

                token_t type = tok.expect(
                        (unsigned)token_t::lpar |
                        (unsigned)token_t::rpar |
                        (unsigned)token_t::atom);                

                switch(type) {
                case token_t::lpar:
                        result = read_list_node(tok);
                        return true;

                case token_t::rpar:
                        return false;

                case token_t::atom:
                        result = { dom_node_t::atom, tok.get_last_atom(), {} };
                        return true;
                }

                throw fatal_error("Inconsistent token_t enum.");
        }

        dom_node build_dom_tree(tokenizer& tok) {
                dom_node result;
                if(!read_maybe_node(tok, result))
                        throw parsing_error("Inexpected right parenthesis token.");
                return result;
        }

} // namespace script
