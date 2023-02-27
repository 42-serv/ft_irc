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
      registered_state()
{
    this->channels.reserve(FT_IRC_CHANNEL_LIMIT_PER_USER);
}

ft::irc::user::~user()
{
}

ft::irc::server& ft::irc::user::get_server() const throw()
{
    return this->server;
}

const std::string& ft::irc::user::get_nick() const throw()
{
    return this->nick;
}

void ft::irc::user::set_nick(const std::string& nick)
{
    this->nick = nick;
}

bool ft::irc::user::change_nick(const std::string& nick)
{
    if (this->nick == nick)
    {
        return true;
    }

    if (this->server.hold_nick(nick))
    {
        this->server.release_nick(this->nick);
        this->set_nick(nick); // TODO: consider lock
        return true;
    }
    else
    {
        return false;
    }
}

const std::string& ft::irc::user::get_username() const throw()
{
    return this->username;
}

void ft::irc::user::set_username(const std::string& username)
{
    this->username = username;
}

const std::string& ft::irc::user::get_hostname() const throw()
{
    return this->hostname;
}

void ft::irc::user::set_hostname(const std::string& hostname)
{
    this->hostname = hostname;
}

const std::string& ft::irc::user::get_servername() const throw()
{
    return this->servername;
}

void ft::irc::user::set_servername(const std::string& servername)
{
    this->servername = servername;
}

const std::string& ft::irc::user::get_realname() const throw()
{
    return this->realname;
}

void ft::irc::user::set_realname(const std::string& realname)
{
    this->realname = realname;
}

bool ft::irc::user::get_mode(user_mode index) const throw()
{
    return this->mode[index];
}

void ft::irc::user::set_mode(user_mode index, bool value) throw()
{
    this->mode[index] = value;
}

bool ft::irc::user::get_register_state(register_state index) const throw()
{
    return this->registered_state[index];
}

void ft::irc::user::set_register_state(register_state index, bool value) throw()
{
    this->registered_state[index] = value;
}

bool ft::irc::user::is_registered() const throw()
{
    return this->registered_state.all();
}

void ft::irc::user::register_user()
{
    set_register_state(ft::irc::user::REGISTER_STATE_COMPLETED, true);

    this->send_message(ft::irc::message("NOTICE") >> "ft_irc"
                                                         << "hello?"
                                                         << "abc");
}

const ft::irc::user::channel_list& ft::irc::user::get_channels() const throw()
{
    return this->channels;
}

ft::irc::user::channel_list::size_type ft::irc::user::get_channel_count() const throw()
{
    return this->channels.size();
}

bool ft::irc::user::is_channel_member(const std::string& channelname) const throw()
{
    return std::find(this->channels.begin(), this->channels.end(), channelname) != this->channels.end();
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

void ft::irc::user::finalize_channels()
{
    // TODO: 모든 방에서 나가기 같은 정리 작업
}

void ft::irc::user::send_message(const ft::irc::message& message) const
{
    this->layer.post_write(ft::make_shared<ft::irc::message>(message));
    this->layer.post_flush();
}

void ft::irc::user::exit_client(const std::string& quit_message) const
{
    static_cast<void>(quit_message);
    this->layer.post_disconnect();
}
