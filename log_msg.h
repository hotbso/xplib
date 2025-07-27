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

#ifndef _LOG_MSG_H_
#define _LOG_MSG_H_

#include <string>

// define this in your plugin, e.g.
// const char* log_msg_prefix = "opensam: ";

extern const char* log_msg_prefix;

// functions
extern void LogMsgImpl(const char *, ...) __attribute__ ((format (printf, 1, 2)));
extern void LogMsgRawImpl(const char *, int, const char *);
extern void LogMsgRawImpl(const char *, int, const std::string& str);

// This macro is used to log messages with a file name and line number.
#ifdef __FILE_NAME__
#define LogMsg(fmt, ...) LogMsgImpl(__FILE_NAME__  ":%d: " fmt, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#define LogMsgRaw(str) LogMsgRawImpl(__FILE_NAME__, __LINE__, str)
#else
#define LogMsg(fmt, ...) LogMsgImpl(__FILE__  ":%d: " fmt, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#define LogMsgRaw(str) LogMsgRawImpl(__FILE__, __LINE__, str)
#endif

#endif
