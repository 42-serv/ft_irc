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
#include <sstream>
#include <string>

ft::irc::user::user(ft::irc::server& server, const ft::shared_ptr<ft::serv::event_layer>& layer)
    : server(server),
      layer(layer),
      nick(),
      username(),
      hostname(layer->get_host()),
      realname(),
      channels(),
      mode(),
      registered_state(),
      away_message(),
      quit_message(),
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
        ft::irc::make_reply_base::set_user_nick(nick);
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

bool ft::irc::user::load_mode(user_mode index) const throw()
{
    synchronized (this->lock.get_read_lock())
    {
        return this->mode[index];
    }
}

void ft::irc::user::store_mode(user_mode index, bool value) throw()
{
    synchronized (this->lock.get_write_lock())
    {
        this->mode[index] = value;
    }
}

std::string ft::irc::user::make_mode_string()
{
    synchronized (this->lock.get_read_lock())
    {
        std::ostringstream oss;
        oss << '+';
        if (this->mode[USER_MODE_OPERATOR])
        {
            oss << 'o';
        }
        if (this->mode[USER_MODE_INVISIBLE])
        {
            oss << 'i';
        }
        if (this->mode[USER_MODE_AWAY])
        {
            oss << 'a';
        }
        return oss.str();
    }
}

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

    server.send_welcome(*this);

    this->send_message(ft::irc::make_reply::user_mode_is(this->make_mode_string()));
}

void ft::irc::user::deregister_from_server()
{
    ft::irc::server& server = this->get_server();

    if (this->is_registered())
    {
        this->set_register_state(ft::irc::user::REGISTER_STATE_COMPLETED, false);

        std::string quit_message = this->load_quit_message();
        if (quit_message.empty())
        {
            // default message
            quit_message = this->load_nick();
        }
        this->notify_message(ft::irc::message("QUIT") << quit_message >> this->make_full_name());

        channel_list channels_snapshot;
        synchronized (this->lock.get_write_lock())
        {
            this->channels.swap(channels_snapshot);
        }
        foreach (channel_list::iterator, it, channels_snapshot)
        {
            const ft::shared_ptr<ft::irc::channel> channel = server.find_channel(*it);
            channel->leave_user(*this);
        }

        server.deregister_user(this->shared_from_this());
    }
    if (this->get_register_state(ft::irc::user::REGISTER_STATE_NICK))
    {
        this->set_register_state(ft::irc::user::REGISTER_STATE_NICK, false);

        server.release_nick(this->load_nick());
    }
}

const std::string& ft::irc::user::get_away_message() const throw()
{
    return this->away_message;
}

void ft::irc::user::set_away_message(const std::string& away_message)
{
    this->away_message = away_message;
}

std::string ft::irc::user::load_away_message() const
{
    synchronized (this->lock.get_read_lock())
    {
        return this->get_away_message();
    }
}

void ft::irc::user::store_away_message(const std::string& away_message)
{
    synchronized (this->lock.get_write_lock())
    {
        this->set_away_message(away_message);
    }
}

void ft::irc::user::reset_away_message() throw()
{
    synchronized (this->lock.get_write_lock())
    {
        this->away_message.clear();
    }
}

const std::string& ft::irc::user::get_quit_message() const throw()
{
    return this->quit_message;
}

void ft::irc::user::set_quit_message(const std::string& quit_message)
{
    this->quit_message = quit_message;
}

std::string ft::irc::user::load_quit_message() const
{
    synchronized (this->lock.get_read_lock())
    {
        return this->get_quit_message();
    }
}

void ft::irc::user::store_quit_message(const std::string& quit_message)
{
    synchronized (this->lock.get_write_lock())
    {
        this->set_quit_message(quit_message);
    }
}

void ft::irc::user::reset_quit_message() throw()
{
    synchronized (this->lock.get_write_lock())
    {
        this->quit_message.clear();
    }
}

ft::irc::user::channel_list ft::irc::user::channel_names_snapshot() const throw()
{
    synchronized (this->lock.get_read_lock())
    {
        return this->channels;
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

bool ft::irc::user::contains_invite(const ft::shared_ptr<channel>& channel) const
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
    try
    {
        const ft::shared_ptr<ft::serv::event_layer>& layer = this->layer.lock();
        layer->post_write(ft::make_shared<ft::irc::message>(message));
        layer->post_flush();
    }
    catch (const ft::bad_weak_ptr&)
    {
        // ignore
    }
}

void ft::irc::user::notify_message(const ft::irc::message& message) const
{
    ft::serv::unique_set<ft::shared_ptr<const ft::irc::user> >::type unique_set;
    this->send_message(message);
    unique_set.insert(this->shared_from_this());

    const channel_list channels_snapshot = this->channel_names_snapshot();
    foreach (channel_list::const_iterator, it, channels_snapshot)
    {
        const ft::shared_ptr<ft::irc::channel> channel = server.find_channel(*it);
        channel->broadcast_unique(message, unique_set);
    }
}

void ft::irc::user::exit_client() const
{
    try
    {
        this->layer.lock()->post_disconnect();
    }
    catch (const ft::bad_weak_ptr&)
    {
        // ignore
    }
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
