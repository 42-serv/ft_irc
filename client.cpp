/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "client.hpp"
#include <algorithm>

ft::irc::client::client(const std::string& nick, const std::string& username, const std::string& hostname, const std::string& servername, const std::string& realname)
    : nick(nick),
      username(username),
      hostname(hostname),
      servername(servername),
      realname(realname),
      channels(),
      mode()
{
}

ft::irc::client::~client()
{
}

const std::string& ft::irc::client::get_nick() const throw()
{
    return this->nick;
}

const std::string& ft::irc::client::get_username() const throw()
{
    return this->username;
}

const std::string& ft::irc::client::get_hostname() const throw()
{
    return this->hostname;
}

const std::string& ft::irc::client::get_servername() const throw()
{
    return this->servername;
}

const std::string& ft::irc::client::get_realname() const throw()
{
    return this->realname;
}

const channel_list& ft::irc::client::get_channels() const throw()
{
    return this->channels;
}

const user_mode& ft::irc::client::get_mode() const throw()
{
    return this->mode;
}

bool ft::irc::client::is_member_of(const std::string& channelname) const throw()
{
    return std::find(this->channels.begin(), this->channels.end(), channelname) != this->channels.end();
}

ft::irc::client::channel_list::size_type ft::irc::client::get_channel_number() const throw()
{
    return this->channels.size();
}

void ft::irc::client::set_nick(const std::string& nick)
{
    this->nick = nick;
}

void ft::irc::client::set_mode(const user_mode& mode)
{
    this->mode = mode;
}

void ft::irc::client::join_channel(const std::string& channelname)
{
    this->channels.push_back(channelname);
}

void ft::irc::client::part_channel(const std::string& channelname)
{
    channel_list::iterator it = std::find(this->channels.begin(), this->channels.end(), channelname);

    if (it != this->channels.end())
    {
        this->channels.erase(it);
    }
}
