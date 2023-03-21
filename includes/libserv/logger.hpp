/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "serv_types.hpp"

#include <thread/mutex.hpp>

#include <sys/time.h> // localtime_r

#include <cstdarg>
#include <ctime> // time_t, time, strftime
#include <iostream>
#include <sstream>
#include <string>

#include <cassert>

#define T_RESET "\033[0m"

#define T_REVERSE "\033[7m"
#define T_RESET_REVERSE "\033[27m"

#define T_FG_BLACK "\033[0;30m"
#define T_FG_RED "\033[0;31m"
#define T_FG_GREEN "\033[0;32m"
#define T_FG_YELLOW "\033[0;33m"
#define T_FG_BLUE "\033[0;34m"
#define T_FG_MAGENTA "\033[0;35m"
#define T_FG_CYAN "\033[0;36m"
#define T_FG_WHITE "\033[0;37m"

#define T_FG_BLACK_BOLD "\033[1;30m"
#define T_FG_RED_BOLD "\033[1;31m"
#define T_FG_GREEN_BOLD "\033[1;32m"
#define T_FG_YELLOW_BOLD "\033[1;33m"
#define T_FG_BLUE_BOLD "\033[1;34m"
#define T_FG_MAGENTA_BOLD "\033[1;35m"
#define T_FG_CYAN_BOLD "\033[1;36m"
#define T_FG_WHITE_BOLD "\033[1;37m"

#define T_BG_BLACK "\033[40m"
#define T_BG_RED "\033[41m"
#define T_BG_GREEN "\033[42m"
#define T_BG_YELLOW "\033[43m"
#define T_BG_BLUE "\033[44m"
#define T_BG_MAGENTA "\033[45m"
#define T_BG_CYAN "\033[46m"
#define T_BG_WHITE "\033[47m"

#define T_FG_DEFAULT "\033[0;39m"
#define T_BG_DEFAULT "\033[0;49m"

#ifndef FT_SERV_WRITE_LOG
#define FT_SERV_WRITE_LOG(msg) std::cout << msg
#endif

namespace ft
{
    namespace serv
    {
        namespace _internal
        {
            inline std::string make_utc_string()
            {
                std::time_t time;
                assert(std::time(&time) != static_cast<std::time_t>(-1));
                ::tm t;
                assert(::localtime_r(&time, &t) != null);
                char time_str[sizeof("yyyy-mm-ddThh:mm:ssZ")];
                assert(std::strftime(time_str, sizeof(time_str), "%Y-%m-%dT%H:%M:%SZ", &t) == sizeof(time_str) - sizeof('\0'));
                return time_str;
            }

            inline std::string make_format_string_line(const char* format, va_list& ap)
            {
                std::ostringstream oss;

                for (; *format != '\0'; format++)
                {
                    if (*format == '%')
                    {
                        format++;
                        if (*format == '\0')
                        {
                            break;
                        }

                        switch (*format)
                        {
                        case '%':
                            oss << '%';
                            break;
                        case 'c':
                            oss << static_cast<char>(va_arg(ap, int));
                            break;
                        case 's':
                            oss << va_arg(ap, const char*);
                            break;
                        case 'd':
                        case 'i':
                            oss << std::dec << va_arg(ap, int);
                            break;
                        case 'o':
                            oss << std::oct << va_arg(ap, unsigned int);
                            break;
                        case 'x':
                            oss << std::nouppercase << std::hex << va_arg(ap, unsigned int);
                            break;
                        case 'X':
                            oss << std::uppercase << std::hex << va_arg(ap, unsigned int);
                            break;
                        case 'u':
                            oss << std::dec << va_arg(ap, unsigned int);
                            break;
                        case 'f':
                            oss << std::nouppercase << std::fixed << va_arg(ap, double);
                            break;
                        case 'F':
                            oss << std::uppercase << std::fixed << va_arg(ap, double);
                            break;
                        case 'e':
                            oss << std::nouppercase << std::scientific << va_arg(ap, double);
                            break;
                        case 'E':
                            oss << std::uppercase << std::scientific << va_arg(ap, double);
                            break;
                        case 'p':
                            oss << va_arg(ap, const void*);
                            break;
                        }
                    }
                    else
                    {
                        oss << *format;
                    }
                }
                oss << std::endl;
                return oss.str();
            }

            inline void print(const char* prefix, const char* format, va_list& ap)
            {
                std::string msg = "\r" T_REVERSE "[" + _internal::make_utc_string() + "]" T_RESET_REVERSE "\t" + prefix + _internal::make_format_string_line(format, ap);
                FT_SERV_WRITE_LOG(msg);
            }
        }

        struct logger
        {
#define BEGIN_VARARG(ap, param) \
    va_list ap;                 \
    va_start(ap, param)
#define END_VARARG(ap) va_end(ap)

            static inline void trace(const char* format, ...)
            {
#ifdef FT_TRACE
                BEGIN_VARARG(ap, format);
                _internal::print(T_FG_CYAN_BOLD "[TRACE]" T_FG_DEFAULT "\t", format, ap);
                END_VARARG(ap);
#else
                static_cast<void>(format);
#endif
            }

            static inline void debug(const char* format, ...)
            {
#ifdef FT_DEBUG
                BEGIN_VARARG(ap, format);
                _internal::print(T_FG_MAGENTA_BOLD "[DEBUG]" T_FG_DEFAULT "\t", format, ap);
                END_VARARG(ap);
#else
                static_cast<void>(format);
#endif
            }

            static inline void info(const char* format, ...)
            {
                BEGIN_VARARG(ap, format);
                _internal::print(T_FG_GREEN_BOLD "[INFO]" T_FG_DEFAULT "\t", format, ap);
                END_VARARG(ap);
            }

            static inline void warn(const char* format, ...)
            {
                BEGIN_VARARG(ap, format);
                _internal::print(T_FG_YELLOW_BOLD "[WARN]" T_FG_DEFAULT "\t", format, ap);
                END_VARARG(ap);
            }

            static inline void error(const char* format, ...)
            {
                BEGIN_VARARG(ap, format);
                _internal::print(T_FG_RED_BOLD "[ERROR]" T_FG_DEFAULT "\t", format, ap);
                END_VARARG(ap);
            }

#undef BEGIN_VARARG
#undef END_VARARG
        };
    }
}
