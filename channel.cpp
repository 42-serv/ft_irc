/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "channel.hpp"

#include "irc_constants.hpp"

#include "message.hpp"
#include "reply.hpp"
#include "server.hpp"
#include "user.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>
#include <thread/readwrite_lock.hpp>

#include <algorithm>
#include <bitset>
#include <string>

ft::irc::channel::channel(ft::irc::server& server, const std::string& name)
    : server(server),
      name(name),
      mode(),
      members(),
      lock(),
      invalidated()
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

const ft::irc::channel::member_list& ft::irc::channel::get_members() const throw()
{
    return this->members;
}

void ft::irc::channel::broadcast(const ft::irc::message& message) const
{
    synchronized (this->lock.get_read_lock())
    {
        foreach (member_list::const_iterator, it, this->members)
        {
            it->user->send_message(message);
        }
    }
}

ft::irc::reply_numerics ft::irc::channel::enter_user(const ft::shared_ptr<ft::irc::user>& user)
{
    synchronized (this->lock.get_write_lock())
    {
        if (this->invalidated)
        {
            return ERR_NOSUCHCHANNEL;
        }

        const bool first = this->members.empty();
        // TODO: check invite, limit, key, ban

        member_list::iterator it = this->members.insert(this->members.end(), member(user));
        if (first)
        {
            it->mode[member::MEMBER_MODE_OWNER] = true;
            it->mode[member::MEMBER_MODE_OPERATOR] = true;
            it->user->send_message(ft::irc::message("NOTICE") >> "ft_irc"
                                                                     << "You're new owner.");
        }
        it->user->send_message(ft::irc::make_reply::topic(this->name, "This is topic"));
    }
    this->broadcast(ft::irc::message("NOTICE") >> "ft_irc"
                                                      << "User " + user->get_nick() + " joined.");
    return RPL_NONE;
}

void ft::irc::channel::leave_user(const ft::shared_ptr<ft::irc::user>& user)
{
    bool remove_channel = false;
    synchronized (this->lock.get_write_lock())
    {
        member_list::iterator it = std::find(this->members.begin(), this->members.end(), user);
        if (it == this->members.end())
        {
            return;
        }
        const bool owner = it->mode[member::MEMBER_MODE_OWNER];
        this->members.erase(it);
        if (this->members.empty())
        {
            this->invalidated = true;
            remove_channel = true;
        }
        else if (owner)
        {
            member& successor = this->members.front();
            successor.mode[member::MEMBER_MODE_OWNER] = true;
            successor.mode[member::MEMBER_MODE_OPERATOR] = true;
            successor.user->send_message(ft::irc::message("NOTICE") >> "ft_irc"
                                                                           << "You're new owner.");
        }
    }
    this->broadcast(ft::irc::message("NOTICE") >> "ft_irc"
                                                      << "User " + user->get_nick() + " leaved.");
    if (remove_channel)
    {
        this->server.remove_channel(this->get_name());
    }
}

ft::irc::channel::member::member(const ft::shared_ptr<ft::irc::user>& user)
    : user(user),
      mode()
{
}

bool ft::irc::channel::member::operator==(ft::shared_ptr<ft::irc::user> that)
{
    return this->user == that;
}
