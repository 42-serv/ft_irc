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
      topic(),
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

const std::string& ft::irc::channel::get_topic() const throw()
{
    return this->topic;
}

void ft::irc::channel::set_topic(const std::string& topic)
{
    this->topic = topic;
}

std::string ft::irc::channel::load_topic() const throw()
{
    synchronized (this->lock.get_read_lock())
    {
        return this->get_topic();
    }
}

void ft::irc::channel::store_topic(const std::string& topic)
{
    synchronized (this->lock.get_write_lock())
    {
        this->set_topic(topic);
    }
}

bool ft::irc::channel::get_mode(channel_mode index) const throw()
{
    return this->mode[index];
}

void ft::irc::channel::set_mode(channel_mode index, bool value) throw()
{
    this->mode[index] = value;
}

bool ft::irc::channel::load_mode(channel_mode index) const throw()
{
    synchronized (this->lock.get_read_lock())
    {
        return this->get_mode(index);
    }
}

void ft::irc::channel::store_mode(channel_mode index, bool value) throw()
{
    synchronized (this->lock.get_write_lock())
    {
        this->set_mode(index, value);
    }
}

std::size_t ft::irc::channel::get_limit() const throw()
{
    return this->limit;
}

void ft::irc::channel::set_limit(std::size_t limit)
{
    this->limit = limit;
}

std::size_t ft::irc::channel::load_limit() const throw()
{
    synchronized (this->lock.get_read_lock())
    {
        return this->get_limit();
    }
}

void ft::irc::channel::store_limit(std::size_t limit)
{
    synchronized (this->lock.get_write_lock())
    {
        this->set_limit(limit);
    }
}

const std::string& ft::irc::channel::get_key() const throw()
{
    return this->key;
}

void ft::irc::channel::set_key(const std::string& key)
{
    this->key = key;
}

std::string ft::irc::channel::load_key() const throw()
{
    synchronized (this->lock.get_read_lock())
    {
        return this->get_key();
    }
}

void ft::irc::channel::store_key(const std::string& key)
{
    synchronized (this->lock.get_write_lock())
    {
        this->set_key(key);
    }
}

ft::irc::reply_numerics ft::irc::channel::change_topic(const ft::shared_ptr<const ft::irc::user>& user, const std::string& new_topic)
{
    synchronized (this->lock.get_write_lock())
    {
        if (this->invalidated)
        {
            return ft::irc::ERR_NOSUCHCHANNEL;
        }

        foreach (member_list::const_iterator, it, this->members)
        {
            if (it->user == user)
            {
                if (this->get_mode(CHANNEL_MODE_TOPIC_LIMIT) && !it->mode[member::MEMBER_MODE_OPERATOR])
                {
                    return ft::irc::ERR_CHANOPRIVSNEEDED;
                }

                this->set_topic(new_topic);
                return ft::irc::RPL_NONE;
            }
        }
    }
    return ft::irc::ERR_NOTONCHANNEL;
}

ft::irc::reply_numerics ft::irc::channel::enter_user(const ft::shared_ptr<ft::irc::user>& user, const std::string& key)
{
    synchronized (this->lock.get_write_lock())
    {
        if (this->invalidated)
        {
            return ft::irc::ERR_NOSUCHCHANNEL;
        }

        if (user->get_mode(ft::irc::user::USER_MODE_OPERATOR)) // TODO: 동시성 고려
        {
            // bypass all restrict
        }
        else
        {
            if (this->get_mode(CHANNEL_MODE_LIMIT) && this->members.size() >= this->get_limit())
            {
                return ft::irc::ERR_CHANNELISFULL;
            }
            if (this->get_mode(CHANNEL_MODE_INVITE_ONLY) && !user->contains_invite(this->shared_from_this()))
            {
                return ft::irc::ERR_INVITEONLYCHAN;
            }
            if (this->get_mode(CHANNEL_MODE_BAN) && this->is_banned(user))
            {
                return ft::irc::ERR_BANNEDFROMCHAN;
            }
            if (this->get_mode(CHANNEL_MODE_KEY) && this->get_key() != key)
            {
                return ft::irc::ERR_BADCHANNELKEY;
            }
        }

        const bool first = this->members.empty();

        member_list::iterator it = this->members.insert(this->members.end(), member(user));
        if (first)
        {
            it->mode[member::MEMBER_MODE_OWNER] = true;
            it->mode[member::MEMBER_MODE_OPERATOR] = true;
            it->user->send_message(ft::irc::make_reply::create("NOTICE") << this->get_name() << "You're new owner.");
        }
    }
    return ft::irc::RPL_NONE;
}

void ft::irc::channel::leave_user(const ft::shared_ptr<ft::irc::user>& user)
{
    std::string remove_channel_name;
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
            remove_channel_name = this->get_name();
        }
        else if (owner)
        {
            member& successor = this->members.front();
            successor.mode[member::MEMBER_MODE_OWNER] = true;
            successor.mode[member::MEMBER_MODE_OPERATOR] = true;
            successor.user->send_message(ft::irc::make_reply::create("NOTICE") << this->get_name() << "You're new owner.");
        }
    }
    if (!remove_channel_name.empty())
    {
        this->server.remove_channel(remove_channel_name);
    }
}

ft::irc::reply_numerics ft::irc::channel::invite_user(const ft::shared_ptr<ft::irc::user>& user)
{
    user->add_invite(this->shared_from_this());
    return ft::irc::RPL_NONE;
}

void ft::irc::channel::send_names(const ft::shared_ptr<const ft::irc::user>& user) const throw()
{
    std::string channel_name;
    bool is_secret_channel, is_private_channel;
    std::vector<ft::irc::member_info> user_list;
    synchronized (this->lock.get_read_lock())
    {
        channel_name = this->get_name();
        is_secret_channel = this->get_mode(CHANNEL_MODE_SECRET);
        is_private_channel = this->get_mode(CHANNEL_MODE_PRIVATE);
        foreach (member_list::const_iterator, it, this->members)
        {
            ft::irc::member_info info;
            info.nickname = it->user->load_nick();
            info.is_chanop = it->mode[member::MEMBER_MODE_OPERATOR];
            info.is_chanspk = it->mode[member::MEMBER_MODE_VOICE];
            user_list.push_back(info);
        }
    }
    user->send_message(ft::irc::make_reply::name_reply(is_secret_channel, is_private_channel, channel_name, user_list));
    user->send_message(ft::irc::make_reply::end_of_names(channel_name));
}

void ft::irc::channel::broadcast(const ft::irc::message& message, ft::shared_ptr<const ft::irc::user> except) const
{
    synchronized (this->lock.get_read_lock())
    {
        foreach (member_list::const_iterator, it, this->members)
        {
            if (it->user != except)
            {
                it->user->send_message(message);
            }
        }
    }
}

void ft::irc::channel::broadcast_unique(const ft::irc::message& message, ft::serv::unique_set<ft::shared_ptr<const ft::irc::user> >::type& unique_set) const
{
    synchronized (this->lock.get_read_lock())
    {
        foreach (member_list::const_iterator, it, this->members)
        {
            if (unique_set.find(it->user) == unique_set.end())
            {
                it->user->send_message(message);
                unique_set.insert(it->user);
            }
        }
    }
}

bool ft::irc::channel::is_banned(const ft::shared_ptr<const ft::irc::user>& user) const
{
    static_cast<void>(user);
    return false; // FIXME: check ban
}

ft::irc::channel::member::member(const ft::shared_ptr<ft::irc::user>& user)
    : user(user),
      mode()
{
}

bool ft::irc::channel::member::operator==(const ft::shared_ptr<const ft::irc::user>& that)
{
    return this->user == that;
}
