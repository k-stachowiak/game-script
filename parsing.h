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

#include <sstream>
#include <string>

namespace script {

static bool parse_literal(
                std::string literal,
                bool& is_int,
                int& int_val,
                double& real_val) {

        std::stringstream ss;
        ss << literal;

        if(literal.find('.') == std::string::npos) {
                is_int = true;
                ss >> int_val;
        } else {
                is_int = false;
                ss >> real_val;
        }

        return ss;
}

} // namespace script
