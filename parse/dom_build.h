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

#ifndef DOM_BUILD_H
#define DOM_BUILD_H

#include "dom.h"

/*
 * This is the API for the DOM builder. It is supposed to take a tokens stream
 * and translate it into a DOM tree. The token structure routines are hidden
 * in this module.
 *
 * Test status : tested.
 */

namespace script
{

    // This takes a non-const argument as a phony ")" is appended at the end
    // of the tokens list for the time of the processing.
    node build_dom_tree(std::vector<std::string>&);

}

#endif
