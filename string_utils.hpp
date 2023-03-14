/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include <libserv/libserv.hpp>

#include <cctype>
#include <string>

namespace ft
{
    namespace irc
    {
        namespace _internal
        {
            static const char upper_table[0x100] = {
                '\0', '\1', '\2', '\3', '\4', '\5', '\6', '\a', '\b', '\t', '\n', '\v', '\f', '\r', '\16', '\17',
                '\20', '\21', '\22', '\23', '\24', '\25', '\26', '\27', '\30', '\31', '\32', '\33', '\34', '\35', '\36', '\37',
                ' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
                '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
                'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
                '`', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
                'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '~', '\177',
                '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87', '\x88', '\x89', '\x8A', '\x8B', '\x8C', '\x8D', '\x8E', '\x8F',
                '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97', '\x98', '\x99', '\x9A', '\x9B', '\x9C', '\x9D', '\x9E', '\x9F',
                '\xA0', '\xA1', '\xA2', '\xA3', '\xA4', '\xA5', '\xA6', '\xA7', '\xA8', '\xA9', '\xAA', '\xAB', '\xAC', '\xAD', '\xAE', '\xAF',
                '\xB0', '\xB1', '\xB2', '\xB3', '\xB4', '\xB5', '\xB6', '\xB7', '\xB8', '\xB9', '\xBA', '\xBB', '\xBC', '\xBD', '\xBE', '\xBF',
                '\xC0', '\xC1', '\xC2', '\xC3', '\xC4', '\xC5', '\xC6', '\xC7', '\xC8', '\xC9', '\xCA', '\xCB', '\xCC', '\xCD', '\xCE', '\xCF',
                '\xD0', '\xD1', '\xD2', '\xD3', '\xD4', '\xD5', '\xD6', '\xD7', '\xD8', '\xD9', '\xDA', '\xDB', '\xDC', '\xDD', '\xDE', '\xDF',
                '\xE0', '\xE1', '\xE2', '\xE3', '\xE4', '\xE5', '\xE6', '\xE7', '\xE8', '\xE9', '\xEA', '\xEB', '\xEC', '\xED', '\xEE', '\xEF',
                '\xF0', '\xF1', '\xF2', '\xF3', '\xF4', '\xF5', '\xF6', '\xF7', '\xF8', '\xF9', '\xFA', '\xFB', '\xFC', '\xFD', '\xFE', '\xFF'};

            static const char lower_table[0x100] = {
                '\0', '\1', '\2', '\3', '\4', '\5', '\6', '\a', '\b', '\t', '\n', '\v', '\f', '\r', '\16', '\17',
                '\20', '\21', '\22', '\23', '\24', '\25', '\26', '\27', '\30', '\31', '\32', '\33', '\34', '\35', '\36', '\37',
                ' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
                '@', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
                'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '^', '_',
                '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
                'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', '\177',
                '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87', '\x88', '\x89', '\x8A', '\x8B', '\x8C', '\x8D', '\x8E', '\x8F',
                '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97', '\x98', '\x99', '\x9A', '\x9B', '\x9C', '\x9D', '\x9E', '\x9F',
                '\xA0', '\xA1', '\xA2', '\xA3', '\xA4', '\xA5', '\xA6', '\xA7', '\xA8', '\xA9', '\xAA', '\xAB', '\xAC', '\xAD', '\xAE', '\xAF',
                '\xB0', '\xB1', '\xB2', '\xB3', '\xB4', '\xB5', '\xB6', '\xB7', '\xB8', '\xB9', '\xBA', '\xBB', '\xBC', '\xBD', '\xBE', '\xBF',
                '\xC0', '\xC1', '\xC2', '\xC3', '\xC4', '\xC5', '\xC6', '\xC7', '\xC8', '\xC9', '\xCA', '\xCB', '\xCC', '\xCD', '\xCE', '\xCF',
                '\xD0', '\xD1', '\xD2', '\xD3', '\xD4', '\xD5', '\xD6', '\xD7', '\xD8', '\xD9', '\xDA', '\xDB', '\xDC', '\xDD', '\xDE', '\xDF',
                '\xE0', '\xE1', '\xE2', '\xE3', '\xE4', '\xE5', '\xE6', '\xE7', '\xE8', '\xE9', '\xEA', '\xEB', '\xEC', '\xED', '\xEE', '\xEF',
                '\xF0', '\xF1', '\xF2', '\xF3', '\xF4', '\xF5', '\xF6', '\xF7', '\xF8', '\xF9', '\xFA', '\xFB', '\xFC', '\xFD', '\xFE', '\xFF'};

            static char _to_upper(char c)
            {
                return upper_table[static_cast<unsigned char>(c)];
            }

            static char _to_lower(char c)
            {
                return lower_table[static_cast<unsigned char>(c)];
            }

            static bool _is_letter(char c)
            {
                return std::isalpha(c);
            }

            static bool _is_digit(char c)
            {
                return std::isdigit(c);
            }

            static bool _is_special(char c)
            {
                return c == '-' || c == '[' || c == ']' || c == '\\' || c == '`' || c == '^' || c == '{' || c == '}';
                // return c == '[' || c == ']' || c == '\\' || c == '`' || c == '_' || c == '^' || c == '{' || c == '|' || c == '}';
            }

            static bool _is_except(char c)
            {
                return c == ' ' || c == '\a' || c == '\0' || c == '\\' || c == '\r' || c == '\n' || c == ',';
            }
        }

        struct string_utils
        {
            static inline bool is_valid_nick(const std::string& nick)
            {
                if (nick.empty())
                {
                    return false;
                }
                if (nick.length() > FT_IRC_NICK_LIMIT)
                {
                    return false;
                }

                foreach (std::string::const_iterator, it, nick)
                {
                    if (it == nick.begin())
                    {
                        if (!_internal::_is_letter(*it))
                        // if (!(_internal::_is_letter(*it) || _internal::_is_special(*it)))
                        {
                            return false;
                        }
                    }
                    else
                    {
                        if (!(_internal::_is_letter(*it) || _internal::_is_digit(*it) || _internal::_is_special(*it)))
                        // if (!(_internal::_is_letter(*it) || _internal::_is_digit(*it) || _internal::_is_special(*it) || *it == '-'))
                        {
                            return false;
                        }
                    }
                }

                return true;
            }

            static inline bool is_valid_channelname(const std::string& channelname)
            {
                if (channelname.empty())
                {
                    return false;
                }
                if (channelname.length() > FT_IRC_CHANNEL_NAME_LIMIT)
                {
                    return false;
                }

                foreach (std::string::const_iterator, it, channelname)
                {
                    if (it == channelname.begin())
                    {
                        if (!(*it == '&' || *it == '#'))
                        {
                            return false;
                        }
                    }
                    else
                    {
                        if (_internal::_is_except(*it))
                        {
                            return false;
                        }
                    }
                }

                return true;
            }

            static inline std::string to_upper(const std::string& s)
            {
                std::string str = s;
                foreach (std::string::iterator, it, str)
                {
                    *it = _internal::_to_upper(*it);
                }
                return str;
            }

            static inline std::string to_lower(const std::string& s)
            {
                std::string str = s;
                foreach (std::string::iterator, it, str)
                {
                    *it = _internal::_to_lower(*it);
                }
                return str;
            }

            static inline std::string pick_nick(const std::string& s)
            {
                std::string::size_type pos = s.find_first_of('!');
                return s.substr(0, pos);
            }
        };
    }
}
