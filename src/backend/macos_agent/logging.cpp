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


#include "logging.h"
#include <algorithm>
#include <unistd.h>


/************************************************
 *
 ************************************************/
int debugLevel()
{
    static int res = (getenv("DEBUG") ? std::max(1, atoi(getenv("DEBUG"))) : 0);
    return res;
}


/************************************************
 *
 ************************************************/
void print(FILE *stream, const char *prefix, const char *fmt, va_list args)
{
    static int n;
    pid_t pid = getpid();

    char buff[21];
    time_t timer;
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);
    strftime(buff, 20, "%Y-%m-%d %H:%M:%S", tm_info);
    fprintf(stream, "%20s [%d] (%d)", buff, pid, ++n);

    if (strlen(prefix))
        fprintf(stream, "%s ", prefix);

    vfprintf(stream, fmt, args);
    fprintf(stream, "\n");
}


/************************************************
 *
 ************************************************/
void debug(const char *str, ...)
{
    if (debugLevel())
    {
        va_list args;
        va_start(args, str);
        print(stdout, "Debug:", str, args);
        va_end(args);
    }
}


/************************************************
 *
 ************************************************/
void info(const char *str, ...)
{
    va_list args;
    va_start(args, str);
    print(stdout, "Info:", str, args);
    va_end(args);
}


/************************************************
 *
 ************************************************/
void warn(const char *str, ...)
{
    va_list args;
    va_start(args, str);
    print(stderr, "Warning:", str, args);
    va_end(args);
}


/************************************************
 *
 ************************************************/
void error(const char *str, ...)
{
    va_list args;
    va_start(args, str);
    print(stderr, "Error:", str, args);
    va_end(args);
}
