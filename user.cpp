/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "user.hpp"

#include "irc_constants.hpp"

#include "message.hpp"
#include "server.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>

#include <algorithm>
#include <bitset>
#include <string>

ft::irc::user::user(ft::irc::server& server, ft::serv::event_layer& layer)
    : server(server),
      layer(layer),
      nick(),
      username(),
      hostname(),
      servername(),
      realname(),
      channels(),
      mode(),
      state()
{
    this->channels.reserve(FT_IRC_CHANNEL_LIMIT_PER_USER);
}

ft::irc::user::~user()
{
}

const std::string& ft::irc::user::get_nick() const throw()
{
    return this->nick;
}

const std::string& ft::irc::user::get_username() const throw()
{
    return this->username;
}

const std::string& ft::irc::user::get_hostname() const throw()
{
    return this->hostname;
}

const std::string& ft::irc::user::get_servername() const throw()
{
    return this->servername;
}

const std::string& ft::irc::user::get_realname() const throw()
{
    return this->realname;
}

const ft::irc::user::channel_list& ft::irc::user::get_channels() const throw()
{
    return this->channels;
}

bool ft::irc::user::get_mode(user_mode index) const throw()
{
    return this->mode[index];
}

bool ft::irc::user::is_member_of(const std::string& channelname) const throw()
{
    return std::find(this->channels.begin(), this->channels.end(), channelname) != this->channels.end();
}

ft::irc::user::channel_list::size_type ft::irc::user::get_channel_number() const throw()
{
    return this->channels.size();
}

void ft::irc::user::set_nick(const std::string& nick)
{
    this->nick = nick;
}

void ft::irc::user::set_mode(user_mode index, bool value)
{
    this->mode[index] = value;
}

void ft::irc::user::join_channel(const std::string& channelname)
{
    this->channels.push_back(channelname);
}

void ft::irc::user::part_channel(const std::string& channelname)
{
    channel_list::iterator it = std::find(this->channels.begin(), this->channels.end(), channelname);

    if (it != this->channels.end())
    {
        this->channels.erase(it);
    }
}

void ft::irc::user::finalize()
{
    // TODO: 모든 방에서 나가기 같은 정리 작업
}

void ft::irc::user::send_message(const ft::irc::message& message) const
{
    this->layer.post_write(ft::make_shared<ft::irc::message>(message));
    this->layer.post_flush();
}
