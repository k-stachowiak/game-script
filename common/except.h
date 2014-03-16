/* Copyright (C) 2013 Krzysztof Stachowiak */

/*
 * This file is part of gme-script.
 *
 * gme-script is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * gme-script is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gme-script. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EXCEPT_H
#define EXCEPT_H

#include <stdexcept>
#include <string>
#include <sstream>

namespace moon 
{
namespace except
{

    // Common.
    // =======

    struct fatal : public std::runtime_error
    {
        static std::string build_msg(const std::string& msg)
        {
            std::stringstream ss;
            ss << "Fatal error: \"" << msg << "\"";
            return ss.str();
        }

        fatal(const std::string& msg)
        : std::runtime_error(build_msg(msg))
        {}
    };


    // Tokenizer related.
    // ==================

    struct unclosed_delimited_token : public std::runtime_error
    {
        static std::string build_msg(unsigned start_offset)
        {
            std::stringstream ss;
            ss << "Unclosed delimited token, open at position: " << start_offset;
            return ss.str();
        }
        
        unclosed_delimited_token(unsigned start)
        : std::runtime_error(build_msg(start))
        {}
    };

    // Dom builder related.
    // ====================

    struct unclosed_dom_list : public std::runtime_error
    {
        unclosed_dom_list()
        : std::runtime_error("Unclosed DOM list")
        {}
    };

    struct empty_atom_string : public std::runtime_error
    {
        empty_atom_string()
        : std::runtime_error("Empty atom string encountered")
        {}
    };

    // Typesystem related.
    // ===================

    struct type_mismatch : public std::runtime_error
    {
        type_mismatch()
        : std::runtime_error("Type mismatch")
        {}
    };
}
}

#endif
