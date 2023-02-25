/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "channel.hpp"

#include "user.hpp"

ft::irc::channel::channel(const std::string& name)
    : name(name)
{
}

ft::irc::channel::~channel()
{
}

const std::string& ft::irc::channel::get_name() const throw()
{
    return this->name;
}

bool ft::irc::channel::get_mode(channel_mode index) const throw()
{
    return this->mode[index];
}

const ft::irc::channel::user_dictionary& ft::irc::channel::get_users() const throw()
{
    return this->users;
}
