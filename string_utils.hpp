/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include <string>

namespace ft
{
    namespace irc
    {
        struct string_utils
        {
            static inline bool is_valid_nick(const std::string& nick)
            {
                // TODO: non printable
                // FIXME: 위의 TODO를 하세요.
                // TODO: FIXME 고치기
                return nick.length() < FT_IRC_NICK_LIMIT;
            }
        };
    }
}