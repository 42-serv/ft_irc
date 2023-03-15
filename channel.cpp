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
#include <sstream>
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

std::string ft::irc::channel::load_topic() const
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

std::string ft::irc::channel::make_mode_string(std::string* param)
{
    synchronized (this->lock.get_read_lock())
    {
        std::ostringstream oss;
        std::ostringstream oss_param;
        oss << '+';
        if (this->mode[CHANNEL_MODE_SECRET])
        {
            oss << 's';
        }
        if (this->mode[CHANNEL_MODE_PRIVATE])
        {
            oss << 'p';
        }
        if (this->mode[CHANNEL_MODE_MODERATED])
        {
            oss << 'm';
        }
        if (this->mode[CHANNEL_MODE_TOPIC_LIMIT])
        {
            oss << 't';
        }
        if (this->mode[CHANNEL_MODE_INVITE_ONLY])
        {
            oss << 'i';
        }
        if (this->mode[CHANNEL_MODE_NO_PRIVMSG])
        {
            oss << 'n';
        }
        if (this->mode[CHANNEL_MODE_LIMIT] && this->limit != 0)
        {
            oss << 'l';
            oss_param << this->limit << ' ';
        }
        if (this->mode[CHANNEL_MODE_KEY] && !this->key.empty())
        {
            oss << 'k';
            oss_param << this->key;
        }
        if (param)
        {
            *param = oss_param.str();
        }
        return oss.str();
    }
}

std::size_t ft::irc::channel::get_limit() const throw()
{
    return this->limit;
}

void ft::irc::channel::set_limit(std::size_t limit) throw()
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

void ft::irc::channel::store_limit(std::size_t limit) throw()
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

std::string ft::irc::channel::load_key() const
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

ft::irc::reply_numerics ft::irc::channel::enter_user(ft::irc::user& user, const std::string& key)
{
    synchronized (this->lock.get_write_lock())
    {
        if (this->invalidated)
        {
            return ft::irc::ERR_NOSUCHCHANNEL;
        }

        if (user.load_mode(ft::irc::user::USER_MODE_OPERATOR))
        {
            // bypass all restrict
        }
        else
        {
            if (this->get_mode(CHANNEL_MODE_LIMIT) && this->get_limit() != 0 && this->members.size() >= this->get_limit())
            {
                return ft::irc::ERR_CHANNELISFULL;
            }
            if (this->get_mode(CHANNEL_MODE_INVITE_ONLY) && !user.contains_invite(this->shared_from_this()))
            {
                return ft::irc::ERR_INVITEONLYCHAN;
            }
            if (this->get_mode(CHANNEL_MODE_BAN) && this->is_banned(user))
            {
                return ft::irc::ERR_BANNEDFROMCHAN;
            }
            if (this->get_mode(CHANNEL_MODE_KEY) && !this->get_key().empty() && this->get_key() != key)
            {
                return ft::irc::ERR_BADCHANNELKEY;
            }
        }

        const bool first = this->members.empty();

        member_list::iterator it = this->members.insert(this->members.end(), member(user.shared_from_this()));
        if (first)
        {
            it->mode[member::MEMBER_MODE_OWNER] = true;
            it->mode[member::MEMBER_MODE_OPERATOR] = true;
        }
    }
    return ft::irc::RPL_NONE;
}

void ft::irc::channel::leave_user(const ft::irc::user& user)
{
    const std::string& channel_name = this->get_name();
    bool remove = false;
    std::string reop_member_name;
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
            remove = true;
        }
        else if (owner)
        {
            member& successor = this->members.front();
            successor.mode[member::MEMBER_MODE_OWNER] = true;
            successor.mode[member::MEMBER_MODE_OPERATOR] = true;
            reop_member_name = successor.user->load_nick();
        }
    }
    if (remove)
    {
        this->server.remove_channel(channel_name);
    }
    else if (!reop_member_name.empty())
    {
        this->broadcast(ft::irc::make_reply::create("MODE") << channel_name << "+o" << reop_member_name);
    }
}

void ft::irc::channel::send_names(const ft::irc::user& user) const throw()
{
    const std::string& channel_name = this->get_name();
    const bool force = user.load_mode(ft::irc::user::USER_MODE_OPERATOR);
    const bool user_is_member = user.is_channel_member(channel_name);
    std::vector<ft::irc::message> user_list_packets;
    synchronized (this->lock.get_read_lock())
    {
        const bool is_secret_channel = this->get_mode(CHANNEL_MODE_SECRET);
        const bool is_private_channel = this->get_mode(CHANNEL_MODE_PRIVATE);
        const std::size_t user_per_page = 32;
        std::vector<ft::irc::member_info> user_list;
        user_list.reserve(user_per_page);
        foreach (member_list::const_iterator, it, this->members)
        {
            if (it->user->load_mode(ft::irc::user::USER_MODE_INVISIBLE))
            {
                if (!force && !user_is_member)
                {
                    continue;
                }
            }

            ft::irc::member_info info;
            info.nickname = it->user->load_nick();
            info.is_chanop = it->mode[member::MEMBER_MODE_OPERATOR];
            info.is_chanspk = it->mode[member::MEMBER_MODE_VOICE];
            user_list.push_back(info);
            if (user_list.size() >= user_per_page)
            {
                user_list_packets.push_back(ft::irc::make_reply::name_reply(is_secret_channel, is_private_channel, channel_name, user_list));
                user_list.clear();
            }
        }
        if (!user_list.empty())
        {
            user_list_packets.push_back(ft::irc::make_reply::name_reply(is_secret_channel, is_private_channel, channel_name, user_list));
        }
        user_list_packets.push_back(ft::irc::make_reply::end_of_names(channel_name));
    }

    foreach (std::vector<ft::irc::message>::const_iterator, it, user_list_packets)
    {
        user.send_message(*it);
    }
}

ft::irc::message ft::irc::channel::make_list_packet(bool force) const throw()
{
    synchronized (this->lock.get_read_lock())
    {
        std::size_t visible_count = 0;
        foreach (member_list::const_iterator, it, this->members)
        {
            if (!it->user->load_mode(ft::irc::user::USER_MODE_INVISIBLE) || force)
            {
                visible_count++;
            }
        }
        return ft::irc::make_reply::list(this->get_name(), visible_count, this->get_topic());
    }
}

void ft::irc::channel::broadcast(const ft::irc::message& message, const ft::irc::user* except) const
{
    synchronized (this->lock.get_read_lock())
    {
        foreach (member_list::const_iterator, it, this->members)
        {
            if (it->user.get() != except)
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

bool ft::irc::channel::is_channel_operator(const ft::irc::user& user) const throw()
{
    synchronized (this->lock.get_read_lock())
    {
        member_list::const_iterator it = std::find(this->members.begin(), this->members.end(), user);
        if (it != this->members.end())
        {
            return it->mode[member::MEMBER_MODE_OPERATOR];
        }
        return false;
    }
}

bool ft::irc::channel::is_channel_speaker(const ft::irc::user& user) const throw()
{
    synchronized (this->lock.get_read_lock())
    {
        member_list::const_iterator it = std::find(this->members.begin(), this->members.end(), user);
        if (it != this->members.end())
        {
            return it->mode[member::MEMBER_MODE_OPERATOR] || it->mode[member::MEMBER_MODE_VOICE];
        }
        return false;
    }
}

void ft::irc::channel::modify_member_mode(const std::string& nick, member::member_mode index, bool value) throw()
{
    synchronized (this->lock.get_write_lock())
    {
        foreach (member_list::iterator, it, this->members)
        {
            if (it->user->load_nick() == nick)
            {
                it->mode[index] = value;
                break;
            }
        }
    }
}

bool ft::irc::channel::is_banned(const ft::irc::user& user) const throw()
{
    static_cast<void>(user);
    return false; // TODO: check ban
}

ft::irc::channel::member::member(const ft::shared_ptr<ft::irc::user>& user)
    : user(user),
      mode()
{
}

bool ft::irc::channel::member::operator==(const ft::shared_ptr<const ft::irc::user>& that) const throw()
{
    return this->user == that;
}

bool ft::irc::channel::member::operator==(const ft::irc::user& that) const throw()
{
    return this->user.get() == &that;
}
