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

#ifndef LOG_H
#define LOG_H

#include <iostream>

namespace moon
{
namespace log 
{
    enum level
    {
        LOG_LVL_TRACE,
        LOG_LVL_INFO,
        LOG_LVL_FATAL
    };

    extern level global_log_level;

    template<typename T>
    void log(level lvl, const T& msg)
    {
        if (lvl < global_log_level)
        {
            return;
        }

        std::cout << msg << std::endl;
        std::cout.flush();
    }

    template<typename Head, typename... Tail>
    void log(level lvl, const Head& hd, Tail... tl, const std::string& sep = " ")
    {
        std::cout << hd << sep;
        log(lvl, tl...);
    }

    template<typename... Args>
    void trace(Args... args)
    {
        log(LOG_LVL_TRACE, args...);
    }

    template<typename... Args>
    void info(Args... args)
    {
        log(LOG_LVL_INFO, args...);
    }

    template<typename... Args>
    void fatal(Args... args)
    {
        log(LOG_LVL_FATAL, args...);
    }
}
}

#endif
