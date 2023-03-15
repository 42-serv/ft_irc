/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "channel.hpp"
#include "irc_constants.hpp"

#include "message.hpp"
#include "processor.hpp"
#include "reply.hpp"
#include "server.hpp"
#include "string_utils.hpp"
#include "user.hpp"

#include <cstddef>
#include <sstream>
#include <string>

namespace ft
{
    namespace irc
    {
        // Command: JOIN
        // Parameters: <channel>{,<channel>} [<key>{,<key>}]
        class processor_join : public ft::irc::processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 1; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                ft::irc::server& server = user.get_server();
                const ft::irc::message::param_vector channel_names = ft::irc::message::split(message[0], ',');

                ft::irc::message::param_vector keys;
                if (message.param_size() > this->get_min_params())
                {
                    keys = ft::irc::message::split(message[1], ',');
                }

                std::size_t i = 0;
                foreach (ft::irc::message::param_vector::const_iterator, it, channel_names)
                {
                    const std::string& channel_name = *it;

                    std::string key;
                    if (i < keys.size())
                    {
                        key = keys[i];
                    }
                    i++;

                    if (user.is_channel_member(channel_name))
                    {
                        continue;
                    }

                    if (!ft::irc::string_utils::is_valid_channelname(channel_name))
                    {
                        user.send_message(ft::irc::make_error::no_such_channel(channel_name));
                        continue;
                    }

                    if (user.channel_count() >= FT_IRC_CHANNEL_LIMIT_PER_USER)
                    {
                        user.send_message(ft::irc::make_error::too_many_channels(channel_name));
                        continue;
                    }

                    const ft::shared_ptr<ft::irc::channel> channel = server.ensure_channel(channel_name);
                    ft::irc::reply_numerics rpl = channel->enter_user(user, key);
                    if (rpl == ft::irc::RPL_NONE)
                    {
                        user.join_channel(channel_name);
                        ft::irc::message payload = ft::irc::make_reply::replicate(message);
                        payload[0] = channel_name;
                        if (!keys.empty())
                        {
                            // mask keys
                            payload[1] = '*';
                        }
                        channel->broadcast(payload);
                        user.remove_invite(channel);
                        std::string topic = channel->load_topic();
                        if (!topic.empty())
                        {
                            user.send_message(ft::irc::make_reply::topic(channel->get_name(), topic));
                        }
                        channel->send_names(user);
                    }
                    else
                    {
                        switch (rpl)
                        {
                        case ft::irc::ERR_NOSUCHCHANNEL:
                            user.send_message(ft::irc::make_error::no_such_channel(channel_name));
                            break;
                        case ft::irc::ERR_CHANNELISFULL:
                            user.send_message(ft::irc::make_error::channel_is_full(channel_name));
                            break;
                        case ft::irc::ERR_INVITEONLYCHAN:
                            user.send_message(ft::irc::make_error::invite_only_channel(channel_name));
                            break;
                        case ft::irc::ERR_BANNEDFROMCHAN:
                            user.send_message(ft::irc::make_error::banned_from_channel(channel_name));
                            break;
                        case ft::irc::ERR_BADCHANNELKEY:
                            user.send_message(ft::irc::make_error::bad_channel_key(channel_name));
                            break;

                        default:
                            assert(false);
                        }
                    }
                }
            }
        };

        // Command: PART
        // Parameters: <channel>{,<channel>}
        class processor_part : public ft::irc::processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 1; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                ft::irc::server& server = user.get_server();
                const ft::irc::message::param_vector channel_names = ft::irc::message::split(message[0], ',');

                foreach (ft::irc::message::param_vector::const_iterator, it, channel_names)
                {
                    const std::string& channel_name = *it;

                    if (user.is_channel_member(channel_name))
                    {
                        user.part_channel(channel_name);

                        const ft::shared_ptr<ft::irc::channel> channel = server.find_channel(channel_name);
                        if (channel)
                        {
                            ft::irc::message payload = ft::irc::make_reply::replicate(message);
                            payload[0] = channel_name;
                            channel->broadcast(payload);
                            channel->leave_user(user);
                        }
                        else
                        {
                            user.send_message(ft::irc::make_error::no_such_channel(channel_name));
                        }
                    }
                    else
                    {
                        user.send_message(ft::irc::make_error::not_on_channel(channel_name));
                    }
                }
            }
        };

        // Command: MODE
        // Parameters: <channel> {[+|-]|o|p|s|i|t|n|b|v} [<limit>] [<user>] [<ban mask>] // channel mode
        // Parameters: <nickname> {[+|-]|i|w|s|o}                                        // user mode
        class processor_mode : public ft::irc::processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 1; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                ft::irc::server& server = user.get_server();
                const std::string& target_name = message[0];
                if (ft::irc::string_utils::is_valid_channelname(target_name))
                {
                    const ft::shared_ptr<ft::irc::channel> channel = server.find_channel(target_name);

                    if (!channel)
                    {
                        user.send_message(ft::irc::make_error::no_such_channel(target_name));
                        return;
                    }

                    if (message.param_size() == this->get_min_params())
                    {
                        // Query only
                        std::string mode_param;
                        std::string mode = channel->make_mode_string(&mode_param);
                        if (!user.is_channel_member(target_name))
                        {
                            // mask
                            mode_param.clear();
                        }
                        user.send_message(ft::irc::make_reply::channel_mode_is(target_name, mode, mode_param));
                    }
                    else
                    {
                        if (!user.is_channel_member(target_name))
                        {
                            user.send_message(ft::irc::make_error::not_on_channel(target_name));
                            return;
                        }

                        const bool can_modify = channel->is_channel_operator(user);
                        const bool modified = this->update_channel_mode(user, channel, message, can_modify);
                        if (can_modify && modified)
                        {
                            channel->broadcast(ft::irc::make_reply::replicate(message));
                        }
                        else if (!can_modify && !modified)
                        {
                            user.send_message(ft::irc::make_error::channel_operator_privileges_needed(target_name));
                        }
                    }
                }
                else
                {
                    const ft::shared_ptr<ft::irc::user> target = server.find_user(target_name);

                    if (!target)
                    {
                        user.send_message(ft::irc::make_error::no_such_nickname(target_name));
                        return;
                    }

                    if (target.get() != &user)
                    {
                        user.send_message(ft::irc::make_error::users_donot_match());
                        return;
                    }

                    if (message.param_size() == this->get_min_params())
                    {
                        // Query only
                        user.send_message(ft::irc::make_reply::user_mode_is(target->make_mode_string()));
                    }
                    else
                    {
                        this->update_user_mode(user, target, message[1]);
                        target->send_message(ft::irc::make_reply::replicate(message));
                    }
                }
            }

            void update_user_mode(ft::irc::user& user, const ft::shared_ptr<ft::irc::user>& target, const std::string& mode_str) const
            {
                enum
                {
                    NONE,
                    ADD,
                    REMOVE
                } state = NONE;
                foreach (std::string::const_iterator, it, mode_str)
                {
                    std::string::value_type c = *it;
                    if (c == '+')
                    {
                        state = ADD;
                    }
                    else if (c == '-')
                    {
                        state = REMOVE;
                    }
                    else
                    {
                        if (state == NONE)
                        {
                            // nothing
                            continue;
                        }

                        bool add = state == ADD;
                        switch (c)
                        {
                        case 'o':
                            if (add)
                            {
                                // ignore self opping
                                continue;
                            }
                            target->store_mode(ft::irc::user::USER_MODE_OPERATOR, add);
                            break;
                        case 'i':
                            target->store_mode(ft::irc::user::USER_MODE_INVISIBLE, add);
                            break;
                        case 'a':
                            target->store_mode(ft::irc::user::USER_MODE_AWAY, add);
                            break;
                        default:
                            user.send_message(ft::irc::make_error::user_mode_unknown_flag());
                        }
                    }
                }
            }

            bool update_channel_mode(ft::irc::user& user, const ft::shared_ptr<ft::irc::channel>& channel, const ft::irc::message& message, bool can_modify) const
            {
                const std::string& mode_str = message[1];
                enum
                {
                    NONE,
                    ADD,
                    REMOVE
                } state = NONE;
                std::size_t param_index = this->get_min_params() + 1;
                foreach (std::string::const_iterator, it, mode_str)
                {
                    std::string::value_type c = *it;
                    if (c == '+')
                    {
                        state = ADD;
                    }
                    else if (c == '-')
                    {
                        state = REMOVE;
                    }
                    else
                    {
                        if (state == NONE)
                        {
                            switch (c)
                            {
                            case 'b':
                                // Query ban list
                                channel->send_ban_list(user);
                                break;
                            }
                            continue;
                        }
                        else if (!can_modify)
                        {
                            return false;
                        }

                        bool add = state == ADD;
                        switch (c)
                        {
                        case 'o':
                            if (message.param_size() <= param_index)
                            {
                                break;
                            }

                            {
                                const std::string& nick = message[param_index++];
                                channel->modify_member_mode(nick, ft::irc::channel::member::MEMBER_MODE_OPERATOR, add);
                            }
                            break;
                        case 'p':
                            channel->store_mode(ft::irc::channel::CHANNEL_MODE_PRIVATE, add);
                            break;
                        case 's':
                            channel->store_mode(ft::irc::channel::CHANNEL_MODE_SECRET, add);
                            break;
                        case 'i':
                            channel->store_mode(ft::irc::channel::CHANNEL_MODE_INVITE_ONLY, add);
                            break;
                        case 't':
                            channel->store_mode(ft::irc::channel::CHANNEL_MODE_TOPIC_LIMIT, add);
                            break;
                        case 'n':
                            channel->store_mode(ft::irc::channel::CHANNEL_MODE_NO_PRIVMSG, add);
                            break;
                        case 'm':
                            channel->store_mode(ft::irc::channel::CHANNEL_MODE_MODERATED, add);
                            break;
                        case 'l':
                            channel->store_mode(ft::irc::channel::CHANNEL_MODE_LIMIT, add);

                            if (add)
                            {
                                if (message.param_size() <= param_index)
                                {
                                    break;
                                }

                                const std::string& arg = message[param_index++];
                                std::istringstream iss(arg);

                                std::size_t limit;
                                iss >> limit;
                                if (!iss)
                                {
                                    limit = 0;
                                }

                                channel->store_limit(limit);
                            }
                            else
                            {
                                channel->store_limit(0);
                            }
                            break;
                        case 'b':
                        {
                            if (message.param_size() <= param_index)
                            {
                                break;
                            }
                            const std::string& mask = message[param_index++];
                            if (add)
                            {
                                channel->ban(mask);
                            }
                            else
                            {
                                channel->unban(mask);
                            }
                            break;
                        }
                        case 'v':
                            if (message.param_size() <= param_index)
                            {
                                break;
                            }

                            {
                                const std::string& nick = message[param_index++];
                                channel->modify_member_mode(nick, ft::irc::channel::member::MEMBER_MODE_VOICE, add);
                            }
                            break;
                        case 'k':
                            channel->store_mode(ft::irc::channel::CHANNEL_MODE_KEY, add);

                            if (add)
                            {
                                if (message.param_size() <= param_index)
                                {
                                    break;
                                }

                                const std::string& key = message[param_index++];

                                if (!key.empty())
                                {
                                    channel->store_key(key);
                                }
                            }
                            else
                            {
                                channel->store_key("");
                            }
                            break;
                        }
                    }
                }
                if (can_modify && state == NONE)
                {
                    return false;
                }
                return true;
            }
        };

        // Command: TOPIC
        // Parameters: <channel> [<topic>]
        class processor_topic : public ft::irc::processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 1; }
            std::size_t get_max_params() const throw() { return 2; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                ft::irc::server& server = user.get_server();
                const std::string& channel_name = message[0];

                const ft::shared_ptr<ft::irc::channel> channel = server.find_channel(channel_name);
                if (channel)
                {
                    if (message.param_size() == this->get_min_params())
                    {
                        std::string topic = channel->load_topic();
                        if (!topic.empty())
                        {
                            user.send_message(ft::irc::make_reply::topic(channel_name, topic));
                        }
                        else
                        {
                            user.send_message(ft::irc::make_reply::no_topic(channel_name));
                        }
                    }
                    else
                    {
                        if (!user.is_channel_member(channel_name))
                        {
                            user.send_message(ft::irc::make_error::not_on_channel(channel_name));
                            return;
                        }

                        if (channel->load_mode(ft::irc::channel::CHANNEL_MODE_TOPIC_LIMIT) && !channel->is_channel_operator(user))
                        {
                            user.send_message(ft::irc::make_error::channel_operator_privileges_needed(channel_name));
                            return;
                        }

                        channel->store_topic(message[1]);
                        channel->broadcast(ft::irc::make_reply::replicate(message));
                    }
                }
                else
                {
                    user.send_message(ft::irc::make_error::no_such_channel(channel_name));
                }
            }
        };

        // Command: NAMES
        // Parameters: [<channel>{,<channel>}]
        class processor_names : public ft::irc::processor_base
        {
        public:
            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                ft::irc::server& server = user.get_server();

                ft::irc::message::param_vector channel_names;
                if (message.param_size() == this->get_min_params())
                {
                    channel_names = user.channel_names_snapshot();
                }
                else
                {
                    channel_names = ft::irc::message::split(message[0], ',');
                }

                foreach (ft::irc::message::param_vector::const_iterator, it, channel_names)
                {
                    const std::string& channel_name = *it;

                    if (!ft::irc::string_utils::is_valid_channelname(channel_name))
                    {
                        continue;
                    }

                    const ft::shared_ptr<ft::irc::channel> channel = server.find_channel(channel_name);
                    if (channel)
                    {
                        channel->send_names(user);
                    }
                }
            }
        };

        // Command: LIST
        // Parameters: [<channel>{,<channel>} [<server>]]
        class processor_list : public ft::irc::processor_base
        {
        public:
            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                ft::irc::server& server = user.get_server();

                ft::irc::message::param_vector channel_names;
                if (message.param_size() == this->get_min_params())
                {
                    channel_names = user.channel_names_snapshot();
                }
                else
                {
                    channel_names = ft::irc::message::split(message[0], ',');
                }

                server.send_list(user, channel_names);
            }
        };

        // Command: INVITE
        // Parameters: <nickname> <channel>
        class processor_invite : public ft::irc::processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 2; }
            std::size_t get_max_params() const throw() { return 2; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                const ft::irc::server& server = user.get_server();

                const std::string& nick = message[0];
                const std::string& channel_name = message[1];

                const ft::shared_ptr<ft::irc::channel> channel = server.find_channel(channel_name);

                if (!channel || !user.is_channel_member(channel_name))
                {
                    user.send_message(ft::irc::make_error::not_on_channel(channel_name));
                    return;
                }

                const ft::shared_ptr<ft::irc::user> target = server.find_user(nick);

                if (!target)
                {
                    user.send_message(ft::irc::make_error::no_such_nickname(nick));
                    return;
                }

                if (target->is_channel_member(channel_name))
                {
                    user.send_message(ft::irc::make_error::user_on_channel(nick, channel_name));
                    return;
                }

                if (channel->load_mode(channel::CHANNEL_MODE_INVITE_ONLY) && !channel->is_channel_operator(user))
                {
                    user.send_message(ft::irc::make_error::channel_operator_privileges_needed(channel_name));
                    return;
                }

                target->add_invite(channel);
                target->send_message(ft::irc::make_reply::replicate(message));
                user.send_message(ft::irc::make_reply::inviting(channel_name, nick));
                if (target->load_mode(user::USER_MODE_AWAY))
                {
                    user.send_message(ft::irc::make_reply::away(nick, target->load_away_message()));
                }
            }
        };

        // Command: KICK
        // Parameters: <channel> <user> [<comment>]
        class processor_kick : public ft::irc::processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 2; }
            std::size_t get_max_params() const throw() { return 3; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                const ft::irc::server& server = user.get_server();

                const std::string& channel_name = message[0];
                const std::string& nick = message[1];
                std::string kick_comment;
                if (message.param_size() == this->get_min_params())
                {
                    // default message
                    kick_comment = "";
                }
                else
                {
                    kick_comment = message[2];
                }

                const ft::shared_ptr<ft::irc::channel> channel = server.find_channel(channel_name);

                if (!channel || !user.is_channel_member(channel_name))
                {
                    user.send_message(ft::irc::make_error::not_on_channel(channel_name));
                    return;
                }

                const ft::shared_ptr<ft::irc::user> target = server.find_user(nick);

                if (!target || !target->is_channel_member(channel_name) || target.get() == &user)
                {
                    user.send_message(ft::irc::make_error::no_such_nickname(nick));
                    return;
                }

                if (!channel->is_channel_operator(user))
                {
                    user.send_message(ft::irc::make_error::channel_operator_privileges_needed(channel_name));
                    return;
                }

                channel->broadcast(ft::irc::make_reply::replicate(message));
                target->part_channel(channel_name);
                channel->leave_user(*target);
            }
        };
    }
}
