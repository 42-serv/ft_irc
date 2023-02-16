/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "reply.hpp"

#include "message.hpp"
#include "reply.hpp"

ft::irc::message ft::irc::make_reply::no_such_nick(nickname_t nickname)
{
    return message(ERR_NOSUCHNICK, this->prefix) << nickname << "No such nick/channel";
}
