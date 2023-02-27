/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "server.hpp"

#include "irc_constants.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>
#include <thread/readwrite_lock.hpp>

#include "channel.hpp"
#include "string_utils.hpp"
#include "user.hpp"

#include <string>
#include <utility>

ft::irc::server::server(const std::string& pass)
    : pass(pass),
      channels(),
      nicks(),
      users(),
      lock()
{
}

ft::irc::server::~server()
{
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
        return this->nicks.insert(std::make_pair(ft::irc::string_utils::to_lower(nick), true)).second;
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
