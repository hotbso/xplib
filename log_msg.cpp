//
//    Copyright (C) 2023, 2025 Holger Teutsch
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
//    USA
//

#include <cstdio>
#include <cstdarg>
#include <cstring>

#include "log_msg.h"

#ifdef LOCAL_DEBUGSTRING
void
XPLMDebugString(const char *str)
{
    fputs(str, stdout); fflush(stdout);
}
#else
#include "XPLMUtilities.h"
#endif

static std::string prefix;
void LogMsgInit(const std::string& pf)
{
    prefix = pf + ": ";
}


static std::string fmt_combined;    // don't allocate again & again

void
LogMsgImpl(const char *fmt, ...)
{
    char line[1024];

    va_list ap;
    va_start(ap, fmt);
    fmt_combined = prefix + fmt + "\n";
    vsnprintf(line, sizeof(line) - 3, fmt_combined.c_str(), ap);
    XPLMDebugString(line);
    va_end(ap);
}
