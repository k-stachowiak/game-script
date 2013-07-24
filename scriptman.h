/* Copyright (C) 2012 Krzysztof Stachowiak */

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

#ifndef SCRIPTMAN_H
#define SCRIPTMAN_H

#include <map>
#include <set>
#include <string>
#include <fstream>

#include "tok.h"
#include "dom.h"

namespace script {

        class scriptman {
                std::map<std::string, dom_node> _doms;
        public:
                scriptman(std::set<std::string> files) {
                        for(auto const& filename : files) {

                                // The binary flag is required on MSW platform, or some
                                // file stream backtracking will be corrupted.
                                std::ifstream in(filename.c_str(), ifstream::binary);

                                if(!in.is_open())
                                        throw resource_not_found_error(
                                                "Couldn't load a configuration file \""
                                                + filename + "\".");

                                tokenizer tok(in);
                                dom_node dom = script::build_dom_tree(tok);

                                in.close();

                                _doms[filename] = dom;
                        }
                }

                dom_node const& get_dom(std::string const& key) const {
                        auto it = _doms.find(key);
                        if(it == end(_doms))
                                throw resource_not_found_error(
                                        "Requested script that hasn't been loaded.");
                        return it->second;
                }
        };

} // namespace script

#endif
