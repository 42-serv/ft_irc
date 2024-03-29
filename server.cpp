/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "server.hpp"

#include "irc_constants.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>
#include <thread/readwrite_lock.hpp>

#include "channel.hpp"
#include "message.hpp"
#include "string_utils.hpp"
#include "user.hpp"

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

static std::string _human_readable_datetime()
{
    std::time_t time;
    assert(std::time(&time) != static_cast<std::time_t>(-1));
    ::tm t;
    assert(::localtime_r(&time, &t) != null);
    char time_str[sizeof("yyyy-mm-dd hh:mm:ss")];
    assert(std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &t) == sizeof(time_str) - sizeof('\0'));
    return time_str;
}

ft::irc::server::server(const std::string& pass)
    : pass(pass),
      channels(),
      nicks(),
      users(),
      lock(),
      created_time(_human_readable_datetime())
{
}

ft::irc::server::~server()
{
}

std::string ft::irc::server::make_full_name() const throw()
{
    synchronized (this->lock.get_read_lock())
    {
        return FT_IRC_SERVER_NAME;
    }
}

std::vector<std::string> ft::irc::server::make_motd_lines() const throw()
{
    synchronized (this->lock.get_read_lock())
    {
        std::vector<std::string> motd;
        FT_IRC_ADD_ALL_MOTD(motd.push_back);
        return motd;
    }
}

std::vector<std::string> ft::irc::server::make_support_params() const throw()
{
    synchronized (this->lock.get_read_lock())
    {
        std::vector<std::string> params;
        FT_IRC_ADD_ALL_SUPPORT_PARAM(params.push_back);
        return params;
    }
}

const std::string& ft::irc::server::get_pass() const throw()
{
    return this->pass;
}

ft::shared_ptr<ft::irc::channel> ft::irc::server::find_channel(const std::string& name) const throw()
{
    synchronized (this->lock.get_read_lock())
    {
        channel_dictionary::const_iterator it = this->channels.find(name);
        if (it != this->channels.end())
        {
            return it->second;
        }
    }
    return ft::shared_ptr<ft::irc::channel>();
}

ft::shared_ptr<ft::irc::channel> ft::irc::server::ensure_channel(const std::string& name)
{
    synchronized (this->lock.get_write_lock())
    {
        ft::shared_ptr<ft::irc::channel>& channel = this->channels[name];
        if (!channel)
        {
            channel = ft::make_shared<ft::irc::channel>(*this, name);
        }
        return channel;
    }
}

void ft::irc::server::remove_channel(const std::string& name)
{
    synchronized (this->lock.get_write_lock())
    {
        this->channels.erase(name);
    }
}

bool ft::irc::server::hold_nick(const std::string& nick)
{
    synchronized (this->lock.get_write_lock())
    {
        return this->nicks.insert(ft::irc::string_utils::to_lower(nick)).second;
    }
}

void ft::irc::server::release_nick(const std::string& nick) throw()
{
    synchronized (this->lock.get_write_lock())
    {
        this->nicks.erase(ft::irc::string_utils::to_lower(nick));
    }
}

ft::shared_ptr<ft::irc::user> ft::irc::server::find_user(const std::string& name) const throw()
{
    synchronized (this->lock.get_read_lock())
    {
        user_list::const_iterator it = std::find_if(this->users.begin(), this->users.end(), ft::irc::user::pred_equals_nick(name));
        if (it != this->users.end())
        {
            return *it;
        }
    }
    return ft::shared_ptr<ft::irc::user>();
}

void ft::irc::server::register_user(const ft::shared_ptr<ft::irc::user>& user)
{
    synchronized (this->lock.get_write_lock())
    {
        this->users.push_back(user);
    }
}

void ft::irc::server::deregister_user(const ft::shared_ptr<ft::irc::user>& user)
{
    synchronized (this->lock.get_write_lock())
    {
        user_list::/*const_*/ iterator it = std::find(this->users.begin(), this->users.end(), user);
        if (it != this->users.end())
        {
            this->users.erase(it);
        }
    }
}

void ft::irc::server::send_welcome(const ft::irc::user& user) const throw()
{
    const std::string servername = this->make_full_name();
    const std::string& version = FT_IRC_VERSION;
    const std::string& umodes = FT_IRC_UMODES;
    const std::string& cmodes = FT_IRC_CMODES;
    const std::vector<std::string> support_params = this->make_support_params();

    user.send_message(ft::irc::make_welcome::welcome("Welcome to the " FT_IRC_NETWORK " Network, " + user.make_full_name()));
    user.send_message(ft::irc::make_welcome::your_host("Your host is " + servername + ", running version " + version));
    user.send_message(ft::irc::make_welcome::created("This server was created " + this->created_time));
    user.send_message(ft::irc::make_welcome::my_info(servername, version, umodes, cmodes));

    const std::size_t param_per_page = 13;
    std::size_t count = support_params.size() / param_per_page + !!(support_params.size() % param_per_page);
    for (std::size_t i = 0; i < count; i++)
    {
        std::vector<std::string>::const_iterator begin = support_params.begin() + i * param_per_page;
        std::vector<std::string>::const_iterator end;
        if (static_cast<std::size_t>(support_params.end() - begin) < param_per_page)
        {
            end = support_params.end();
        }
        else
        {
            end = begin + param_per_page;
        }
        user.send_message(ft::irc::make_welcome::i_support(begin, end));
    }

    int user_count = 0;
    int invisible_count = 0;
    int server_count = 0;
    int operator_count = 0;
    int unknown_count = 0;
    int channel_count = 0;
    int my_client_count = 0;
    int my_server_count = 0;

    synchronized (this->lock.get_read_lock())
    {
        user_count = this->users.size();
        foreach (user_list::const_iterator, it, this->users)
        {
            const ft::shared_ptr<ft::irc::user>& u = *it;
            if (u->load_mode(ft::irc::user::USER_MODE_INVISIBLE))
            {
                invisible_count++;
            }
            if (u->load_mode(ft::irc::user::USER_MODE_OPERATOR))
            {
                operator_count++;
            }
        }
        channel_count = this->channels.size();
        my_client_count = user_count + server_count + unknown_count;
    }

    user.send_message(ft::irc::make_reply::luser_client(user_count, invisible_count, server_count));
    if (operator_count > 0)
    {
        user.send_message(ft::irc::make_reply::luser_operator(operator_count));
    }
    if (unknown_count > 0)
    {
        user.send_message(ft::irc::make_reply::luser_unknown(unknown_count));
    }
    user.send_message(ft::irc::make_reply::luser_channels(channel_count));
    user.send_message(ft::irc::make_reply::luser_me(my_client_count, my_server_count));

    const std::vector<std::string> motd = this->make_motd_lines();
    if (motd.empty())
    {
        user.send_message(ft::irc::make_error::no_motd());
    }
    else
    {
        user.send_message(ft::irc::make_reply::motd_start(this->make_full_name()));
        foreach (std::vector<std::string>::const_iterator, it, motd)
        {
            user.send_message(ft::irc::make_reply::motd(*it));
        }
        user.send_message(ft::irc::make_reply::end_of_motd());
    }
}

void ft::irc::server::send_list(const ft::irc::user& user, const ft::irc::message::param_vector& query) const throw()
{
    const bool force = user.load_mode(ft::irc::user::USER_MODE_OPERATOR);
    user.send_message(ft::irc::make_reply::list_start());
    if (query.empty())
    {
        std::vector<ft::irc::message> messages;

        synchronized (this->lock.get_read_lock())
        {
            foreach (channel_dictionary::const_iterator, it, this->channels)
            {
                const ft::shared_ptr<ft::irc::channel>& channel = it->second;
                const bool user_is_member = user.is_channel_member(channel->get_name());
                const bool public_channel = !channel->load_mode(ft::irc::channel::CHANNEL_MODE_SECRET) && !channel->load_mode(ft::irc::channel::CHANNEL_MODE_PRIVATE);
                if (force || user_is_member || public_channel)
                {
                    messages.push_back(channel->make_list_packet(force || user_is_member));
                }
            }
        }

        foreach (std::vector<ft::irc::message>::const_iterator, it, messages)
        {
            user.send_message(*it);
        }
    }
    else
    {
        foreach (ft::irc::message::param_vector::const_iterator, it, query)
        {
            const ft::shared_ptr<ft::irc::channel> channel = this->find_channel(*it);
            if (channel)
            {
                user.send_message(channel->make_list_packet(force));
            }
        }
    }
    user.send_message(ft::irc::make_reply::list_end());
}

void ft::irc::server::broadcast_all(const ft::irc::message& message, ft::shared_ptr<const ft::irc::user> except) const
{
    synchronized (this->lock.get_read_lock())
    {
        foreach (user_list::const_iterator, it, this->users)
        {
            const ft::shared_ptr<const ft::irc::user>& user = *it;
            if (user != except)
            {
                user->send_message(message);
            }
        }
    }
}

ft::irc::reply_numerics ft::irc::server::check_signature(const std::string& user, const std::string& pass) const
{
    if (user != FT_IRC_OPERATOR_NAME)
    {
        return ft::irc::ERR_NOOPERHOST;
    }
    if (pass != FT_IRC_OPERATOR_PASS)
    {
        return ft::irc::ERR_PASSWDMISMATCH;
    }
    return ft::irc::RPL_NONE;
}
