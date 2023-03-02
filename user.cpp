/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "user.hpp"

#include "irc_constants.hpp"

#include "channel.hpp"
#include "message.hpp"
#include "reply.hpp"
#include "server.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>
#include <thread/readwrite_lock.hpp>

#include <algorithm>
#include <bitset>
#include <string>

ft::irc::user::user(ft::irc::server& server, const ft::shared_ptr<ft::serv::event_layer>& layer)
    : server(server),
      layer(layer),
      nick(),
      username(),
      realname(),
      channels(),
      mode(),
      registered_state(),
      lock()
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

std::string ft::irc::user::load_nick() const
{
    synchronized (this->lock.get_read_lock())
    {
        return this->nick;
    }
}

bool ft::irc::user::change_nick(const std::string& nick)
{
    std::string old_nick = this->load_nick();
    if (old_nick == nick)
    {
        return true;
    }

    if (this->server.hold_nick(nick))
    {
        this->server.release_nick(old_nick);
        synchronized (this->lock.get_write_lock())
        {
            this->nick = nick;
        }
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

std::string ft::irc::user::load_username() const throw()
{
    synchronized (this->lock.get_read_lock())
    {
        return this->username;
    }
}

const std::string& ft::irc::user::get_hostname() const throw()
{
    return this->layer->get_host();
}

std::string ft::irc::user::make_full_name() const throw()
{
    synchronized (this->lock.get_read_lock())
    {
        return this->get_nick() + "!" + this->get_username() + "@" + this->get_hostname();
    }
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

// TODO: 필요한가?
// FT_SERV_DEFINE_TASK_3(task_user_set_mode,
//                       ft::shared_ptr<ft::irc::user>, user,
//                       ft::irc::user::user_mode, index,
//                       bool, value,
//                       user->set_mode(index, value));

// void ft::irc::user::post_set_mode(user_mode index, bool value)
// {
//     this->layer->invoke_task(ft::make_shared<task_user_set_mode>(this->shared_from_this(), index, value));
// }

bool ft::irc::user::get_register_state(register_state index) const throw()
{
    // assert(is_in_event_loop());

    return this->registered_state[index];
}

void ft::irc::user::set_register_state(register_state index, bool value) throw()
{
    // assert(is_in_event_loop());

    this->registered_state[index] = value;
}

bool ft::irc::user::is_registered() const throw()
{
    // assert(is_in_event_loop());

    return this->registered_state.all();
}

void ft::irc::user::register_to_server()
{
    ft::irc::server& server = this->get_server();

    server.register_user(this->shared_from_this());
    this->set_register_state(ft::irc::user::REGISTER_STATE_COMPLETED, true);

    // FIXME: fill data
    int user_count = 42;
    int invisible_count = 42;
    int server_count = 42;
    int operator_count = 42;
    int unknown_count = 42;
    int channel_count = 42;
    int my_client_count = 42;
    int my_server_count = 42;
    this->send_message(ft::irc::make_reply::luser_client(user_count, invisible_count, server_count));
    if (operator_count > 0)
    {
        this->send_message(ft::irc::make_reply::luser_operator(operator_count));
    }
    if (unknown_count > 0)
    {
        this->send_message(ft::irc::make_reply::luser_unknown(unknown_count));
    }
    this->send_message(ft::irc::make_reply::luser_channels(channel_count));
    this->send_message(ft::irc::make_reply::luser_me(my_client_count, my_server_count));

    if (true)
    {
        this->send_message(ft::irc::make_reply::motd_start("irc.42seoul.kr"));
        this->send_message(ft::irc::make_reply::motd("example motd"));
        this->send_message(ft::irc::make_reply::end_of_motd());
    }
    else
    {
        this->send_message(ft::irc::make_error::no_motd());
    }

    // this->send_message(ft::irc::make_reply::user_mode_is("+Oexamplo"));
}

void ft::irc::user::deregister_from_server()
{
    ft::irc::server& server = this->get_server();

    if (this->is_registered())
    {
        this->set_register_state(ft::irc::user::REGISTER_STATE_COMPLETED, false);

        channel_list channels_snapshot;
        synchronized (this->lock.get_write_lock())
        {
            this->channels.swap(channels_snapshot);
        }
        foreach (channel_list::iterator, it, channels_snapshot)
        {
            ft::shared_ptr<ft::irc::channel> channel = server.find_channel(*it);
            channel->leave_user(this->shared_from_this());
        }

        server.deregister_user(this->shared_from_this());
    }
    if (this->get_register_state(ft::irc::user::REGISTER_STATE_NICK))
    {
        this->set_register_state(ft::irc::user::REGISTER_STATE_NICK, false);

        server.release_nick(this->load_nick());
    }
}

ft::irc::user::channel_list::size_type ft::irc::user::channel_count() const throw()
{
    synchronized (this->lock.get_read_lock())
    {
        return this->channels.size();
    }
}

bool ft::irc::user::is_channel_member(const std::string& channelname) const throw()
{
    synchronized (this->lock.get_read_lock())
    {
        return std::find(this->channels.begin(), this->channels.end(), channelname) != this->channels.end();
    }
}

void ft::irc::user::join_channel(const std::string& channelname)
{
    synchronized (this->lock.get_write_lock())
    {
        this->channels.push_back(channelname);
    }
}

void ft::irc::user::part_channel(const std::string& channelname)
{
    synchronized (this->lock.get_write_lock())
    {
        channel_list::iterator it = std::find(this->channels.begin(), this->channels.end(), channelname);

        if (it != this->channels.end())
        {
            this->channels.erase(it);
        }
    }
}

bool ft::irc::user::contains_invite(const ft::shared_ptr<channel>& channel)
{
    synchronized (this->lock.get_read_lock())
    {
        return std::find(this->invites.begin(), this->invites.end(), channel.get()) != this->invites.end();
    }
}

void ft::irc::user::add_invite(const ft::shared_ptr<channel>& channel)
{
    synchronized (this->lock.get_write_lock())
    {
        this->invites.push_back(channel.get());
    }
}

void ft::irc::user::remove_invite(const ft::shared_ptr<channel>& channel)
{
    synchronized (this->lock.get_write_lock())
    {
        invite_list::iterator it = std::find(this->invites.begin(), this->invites.end(), channel.get());

        if (it != this->invites.end())
        {
            this->invites.erase(it);
        }
    }
}

void ft::irc::user::send_message(const ft::irc::message& message) const
{
    this->layer->post_write(ft::make_shared<ft::irc::message>(message));
    this->layer->post_flush();
}

void ft::irc::user::notify_message(const ft::irc::message& message) const
{
    synchronized (this->lock.get_write_lock())
    {
        ft::serv::unique_set<ft::shared_ptr<const ft::irc::user> >::type unique_set;
        send_message(message);
        unique_set.insert(this->shared_from_this());
        foreach (channel_list::const_iterator, it, this->channels)
        {
            ft::shared_ptr<ft::irc::channel> channel = server.find_channel(*it);
            channel->broadcast_unique(message, unique_set);
        }
    }
}

void ft::irc::user::exit_client() const
{
    this->layer->post_disconnect();
}

ft::irc::user::pred_equals_nick::pred_equals_nick(const std::string& nick) throw()
    : nick(nick)
{
}

bool ft::irc::user::pred_equals_nick::operator()(const ft::irc::user& user) const throw()
{
    return this->nick == user.load_nick();
}

bool ft::irc::user::pred_equals_nick::operator()(const ft::irc::user* user) const throw()
{
    return this->nick == user->load_nick();
}

bool ft::irc::user::pred_equals_nick::operator()(const ft::shared_ptr<const ft::irc::user>& user) const throw()
{
    return this->nick == user->load_nick();
}