/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "channel.hpp"

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

const ft::irc::channel::channel_mode& ft::irc::channel::get_mode() const throw()
{
    return this->mode;
}

const ft::irc::channel::client_dictionary& ft::irc::channel::get_clients() const throw()
{
    return this->clients;
}
