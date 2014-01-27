/* Copyright (C) 2014 Krzysztof Stachowiak */

/*
 * This file is part of moon.
 *
 * moon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * moon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with moon. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LOGG_H
#define LOGG_H

#define LLVL_TRACE 0
#define LLVL_DEBUG 1
#define LLVL_ERROR 2

#define LOG_LEVEL LLVL_DEBUG

#if LOG_LEVEL <= LLVL_TRACE
#       define LOG_TRACE(FORMAT, ...) printf("[TRACE] %s:%d " FORMAT "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#       define LOG_TRACE(...)
#endif

#if LOG_LEVEL <= LLVL_DEBUG
#       define LOG_DEBUG(FORMAT, ...) printf("[DEBUG] %s:%d " FORMAT "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#       define LOG_DEBUG
#endif

#if LOG_LEVEL <= LLVL_ERRROR
#       define LOG_ERROR(FORMAT, ...) fprintf(stderr, "[ERROR] %s:%d " FORMAT "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#       define LOG_ERROR
#endif

#endif
