/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "processor_base.hpp"

ft::irc::processor_base::~processor_base()
{
}

int ft::irc::processor_base::get_min_params() const throw()
{
    return 0;
}

int ft::irc::processor_base::get_max_params() const throw()
{
    return FT_IRC_MESSAGE_PARAM_LIMIT;
}

bool ft::irc::processor_base::is_registered_only() const throw()
{
    // NOTE: false when command is { NICK, QUIT, USER, PASS }
    return true;
}

bool ft::irc::processor_base::is_operator_only() const throw()
{
    // NOTE: true when command is { KILL, SQUIT, CONNECT, REHASH, RESTART }
    return false;
}