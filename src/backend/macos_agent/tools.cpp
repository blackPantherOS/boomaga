/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 *
 * Copyright: 2012-2018 Boomaga team https://github.com/Boomaga
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#include "tools.h"
#include <libgen.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

/************************************************
 *
 ************************************************/
std::string basename(const std::string &path)
{
    char *s = (char*)alloca(path.length());
    if (!s)
        throw string("can't extract basename from " + path);

    strcpy(s, path.c_str());
    return string(basename(s));
}


/************************************************
 *
 ************************************************/
void mkdirs(const string &path, mode_t mode)
{
    size_t pos = 0;
    do
    {
        pos = path.find_first_of("/", pos + 1);
        if (mkdir(path.substr(0, pos).c_str(), mode) < 0)
        {
            if (errno != EEXIST)
                throw string("Can't create directory " + path.substr(0, pos) + ":" + strerror(errno));
        }
    } while (pos != std::string::npos);
}


/************************************************
 *
 ************************************************/
string format(const char *fmt, ...)
{
     va_list args;
     va_start(args, fmt);

     char *s;
     vasprintf(&s, fmt, args);
     std::unique_ptr<char, decltype(free)&> up(s, free);
     return std::string(s);
}
