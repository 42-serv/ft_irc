/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "channel.hpp"

#include "irc_constants.hpp"

#include "message.hpp"
#include "user.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>
#include <thread/readwrite_lock.hpp>

#include <algorithm>
#include <bitset>
#include <string>

ft::irc::channel::channel(const std::string& name)
    : name(name),
      mode(),
      users(),
      lock()
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

void ft::irc::channel::broadcast(const ft::irc::message& message) const
{
    foreach (user_dictionary::const_iterator, it, this->users)
    {
        it->second.first->send_message(message);
    }
}
