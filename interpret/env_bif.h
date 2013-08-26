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

#ifndef ENV_BIF_H
#define ENV_BIF_H

#include "env.h"

/*
 * This module is responsible for the construction of a special environment
 * providing all the built in resources such as the built in functions.
 * This include for example the arithmetic operations.
 *
 * Test status : not tested.
 */

namespace script
{

    environment env_create_bif();

}

#endif
