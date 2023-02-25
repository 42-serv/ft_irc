/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "server.hpp"

#include "channel.hpp"
#include "user.hpp"

ft::irc::server::server()
    : pass(),
      channels()
{
}

ft::irc::server::~server()
{
}

ft::shared_ptr<ft::irc::channel> ft::irc::server::get_or_new_channel(const std::string& name)
{
    ft::shared_ptr<ft::irc::channel>& channel = this->channels[name];
    if (!channel)
    {
        channel = ft::make_shared<ft::irc::channel, std::string>(name);
    }
    return channel;
}

void ft::irc::server::remove_channel(const std::string& name)
{
    this->channels.erase(name);
}
