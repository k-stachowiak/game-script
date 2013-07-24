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

#ifndef TOK_H
#define TOK_H

#include <cctype>

#include <string>
using std::string;

#include <sstream>
using std::stringstream;

#include <iostream>
using std::cout;
using std::endl;
using std::istream;
using std::ios_base;

#include "../misc/exceptions.h"

namespace script {

        enum class token_t : unsigned {
                lpar = 1 << 0,
                rpar = 1 << 1,
                atom = 1 << 2
        };

        class tokenizer {
                istream& _in;
                string _last_atom;

        public:
                tokenizer(istream& in) : _in(in), _last_atom("") {}
                operator bool() { return _in; }
                string get_last_atom() const { return _last_atom; }

                // Low level interface.
                // --------------------

                template<class LParCallback, class RParCallback, class AtomCallback>
                bool seek_next(LParCallback on_lpar, RParCallback on_rpar, AtomCallback on_atom) {

                        enum class tok_state {
                                in_atom,
                                in_wspace,
                                in_comment
                        } state = tok_state::in_wspace;

                        stringstream ss;
                        char c;

                        for(;;) {

                                if((c = _in.get()) == EOF) {
                                        return false;
                                }

                                switch(state) {

                                case tok_state::in_atom:

                                        if(iswspace(c)) {
                                                _last_atom = ss.str();
                                                on_atom(_last_atom);
                                                state = tok_state::in_wspace;
                                                return true;
                                        }

                                        if(c == '#') {
                                                _last_atom = ss.str();
                                                on_atom(_last_atom);
                                                state = tok_state::in_comment;
                                                return true;
                                        }

                                        if(c == '(' || c == ')') {
                                                _last_atom = ss.str();
                                                _in.seekg(-1, ios_base::cur);
                                                on_atom(_last_atom);
                                                state = tok_state::in_wspace;
                                                return true;
                                        }

                                        ss << c;
                                        break;

                                case tok_state::in_wspace:

                                        if(iswspace(c))        {
                                                continue;
                                        }

                                        if(c == '#') {
                                                state = tok_state::in_comment;
                                                continue;
                                        }

                                        if(c == '(') {
                                                on_lpar();
                                                return true;
                                        }

                                        if(c == ')') {
                                                on_rpar();
                                                return true;
                                        }

                                        ss << c;
                                        state = tok_state::in_atom;
                                        break;

                                case tok_state::in_comment:
                                        if(c == '\n') {
                                                state = tok_state::in_wspace;
                                                continue;
                                        }
                                        break;
                                }
                        }
                }

                // High level interface.
                // ---------------------

                token_t expect(unsigned int flags) {
                        token_t result = token_t::lpar;
                        seek_next(
                                [&result, flags]() mutable {
                                        if(!(flags & (unsigned int)token_t::lpar)) {
                                                throw parsing_error("Unexpected \"(\" encountered.");
                                        }
                                        result = token_t::lpar;
                                },
                                [&result, flags]() mutable {
                                        if(!(flags & (unsigned int)token_t::rpar)) {
                                                throw parsing_error("Unexpected \")\" encountered.");
                                        }
                                        result = token_t::rpar;
                                },
                                [&result, flags](string const&) mutable {
                                        if(!(flags & (unsigned int)token_t::atom)) {
                                                throw parsing_error("Unexpected \")\" encountered.");
                                        }
                                        result = token_t::atom;
                                });
                        return result;
                }

                void expect_atom(string const& atom) {
                        seek_next(
                                []() { throw parsing_error("Unexpected \"(\" encountered."); },
                                []() { throw parsing_error("Unexpected \")\" encountered."); },
                                [&atom](string const& str) {
                                        if(str != atom) {
                                                throw parsing_error("Unexpected \"" + str + "\" encountered.");
                                        }
                                });
                }
        };

} // namespace script

#endif
