/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "message.hpp"

#include "irc_constants.hpp"

#include <sstream>
#include <string>

template <typename T>
ft::irc::message& ft::irc::message::operator<<(const T& chr)
{
    std::ostringstream oss;
    oss << chr;
    return this->operator<<(oss.str());
}
