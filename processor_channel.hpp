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
                const ft::irc::message::param_vector channelnames = ft::irc::message::split(message[0], ',');

                ft::irc::message::param_vector keys;
                if (message.param_size() > 1)
                {
                    keys = ft::irc::message::split(message[1], ',');
                }

                std::size_t i = 0;
                foreach (ft::irc::message::param_vector::const_iterator, it, channelnames)
                {
                    const std::string& channelname = *it;

                    std::string key;
                    if (i < keys.size())
                    {
                        key = keys[i];
                    }
                    i++;

                    if (user.is_channel_member(channelname))
                    {
                        continue;
                    }

                    if (!ft::irc::string_utils::is_valid_channelname(channelname))
                    {
                        user.send_message(ft::irc::make_error::no_such_channel(channelname));
                        continue;
                    }

                    if (user.channel_count() >= FT_IRC_CHANNEL_LIMIT_PER_USER)
                    {
                        user.send_message(ft::irc::make_error::too_many_channels(channelname));
                        continue;
                    }

                    ft::shared_ptr<ft::irc::channel> channel = server.ensure_channel(channelname);
                    ft::irc::reply_numerics rpl = channel->enter_user(user, key);
                    if (rpl == ft::irc::RPL_NONE)
                    {
                        user.join_channel(channelname);
                        ft::irc::message payload = ft::irc::make_reply::replicate(message);
                        payload[0] = channelname;
                        if (message.param_size() > 1)
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
                            user.send_message(ft::irc::make_error::no_such_channel(channelname));
                            break;
                        case ft::irc::ERR_CHANNELISFULL:
                            user.send_message(ft::irc::make_error::channel_is_full(channelname));
                            break;
                        case ft::irc::ERR_INVITEONLYCHAN:
                            user.send_message(ft::irc::make_error::invite_only_channel(channelname));
                            break;
                        case ft::irc::ERR_BANNEDFROMCHAN:
                            user.send_message(ft::irc::make_error::banned_from_channel(channelname));
                            break;
                        case ft::irc::ERR_BADCHANNELKEY:
                            user.send_message(ft::irc::make_error::bad_channel_key(channelname));
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
                const ft::irc::message::param_vector channelnames = ft::irc::message::split(message[0], ',');

                foreach (ft::irc::message::param_vector::const_iterator, it, channelnames)
                {
                    const std::string& channelname = *it;

                    if (user.is_channel_member(channelname))
                    {
                        user.part_channel(channelname);

                        ft::shared_ptr<ft::irc::channel> channel = server.find_channel(channelname);
                        if (channel)
                        {
                            ft::irc::message payload = ft::irc::make_reply::replicate(message);
                            payload[0] = channelname;
                            channel->broadcast(payload);
                            channel->leave_user(user);
                        }
                        else
                        {
                            user.send_message(ft::irc::make_error::no_such_channel(channelname));
                        }
                    }
                    else
                    {
                        user.send_message(ft::irc::make_error::not_on_channel(channelname));
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
                // FIXME: implement
                ft::irc::server& server = user.get_server();
                const std::string& target_name = message[0];
                if (ft::irc::string_utils::is_valid_channelname(target_name))
                {
                    ft::shared_ptr<ft::irc::channel> channel = server.find_channel(target_name);
                    if (message.param_size() == this->get_min_params())
                    {
                        // Read only
                    }
                    else
                    {
                        this->update_channel_mode(channel, message);
                    }
                }
                else
                {
                }
            }

            void update_channel_mode(const ft::shared_ptr<ft::irc::channel>& channel, const ft::irc::message& message) const
            {
                const std::string& mode_str = message[1];
                enum
                {
                    NONE,
                    ADD,
                    REMOVE
                } state = NONE;
                std::size_t param_index = this->get_min_params();
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
                            // ERROR!
                            break;
                        }

                        bool add = state == ADD;
                        switch (c)
                        {
                        case 'o':
                            // give/take chanop
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
                            // limit
                            if (message.param_size() < param_index)
                            {
                                // 매개변수 부족
                                break;
                            }
                            message[param_index++];
                            break;
                        case 'b':
                            // ban
                            break;
                        case 'v':
                            // give/take speak
                            break;
                        case 'k':
                            // key
                            break;
                        }
                    }
                }
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
                const std::string& channelname = message[0];

                ft::shared_ptr<ft::irc::channel> channel = server.find_channel(channelname);
                if (channel)
                {
                    if (message.param_size() <= 1)
                    {
                        std::string topic = channel->load_topic();
                        if (!topic.empty())
                        {
                            user.send_message(ft::irc::make_reply::topic(channelname, topic));
                        }
                        else
                        {
                            user.send_message(ft::irc::make_reply::no_topic(channelname));
                        }
                    }
                    else
                    {
                        ft::irc::reply_numerics rpl = channel->change_topic(user, message[1]);
                        if (rpl == ft::irc::RPL_NONE)
                        {
                            channel->broadcast(ft::irc::make_reply::replicate(message));
                        }
                        else
                        {
                            switch (rpl)
                            {
                            case ft::irc::ERR_NOSUCHCHANNEL:
                                user.send_message(ft::irc::make_error::no_such_channel(channelname));
                                break;

                            case ft::irc::ERR_NOTONCHANNEL:
                                user.send_message(ft::irc::make_error::not_on_channel(channelname));
                                break;
                            case ft::irc::ERR_CHANOPRIVSNEEDED:
                                user.send_message(ft::irc::make_error::channel_operator_privileges_needed(channelname));
                                break;

                            default:
                                assert(false);
                            }
                        }
                    }
                }
                else
                {
                    user.send_message(ft::irc::make_error::no_such_channel(channelname));
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

                    ft::shared_ptr<ft::irc::channel> channel = server.find_channel(channel_name);
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
                const std::string& nickname = message[0];
                const std::string& channelname = message[1];
                const ft::irc::server& server = user.get_server();
                shared_ptr<ft::irc::user> target_user = server.find_user(nickname);
                shared_ptr<ft::irc::channel> channel = server.find_channel(channelname);

                if (user.is_channel_member(channelname) == false)
                {
                    user.send_message(ft::irc::make_error::not_on_channel(nickname));
                }
                else if (!target_user)
                {
                    user.send_message(ft::irc::make_error::no_such_nickname(nickname));
                }
                else if (target_user->is_channel_member(channelname) == true)
                {
                    user.send_message(ft::irc::make_error::user_on_channel(nickname, channelname));
                }
                else if (channel->get_mode(channel::CHANNEL_MODE_INVITE_ONLY) == true && channel->is_channel_operator(user) == false)
                {
                    user.send_message(ft::irc::make_error::channel_operator_privileges_needed(nickname));
                }
                else
                {
                    if (target_user->get_mode(user::USER_MODE_AWAY) == true)
                    {
                        user.send_message(ft::irc::make_reply::away(nickname, target_user->get_away_message()));
                    }
                    else
                    {
                        user.send_message(ft::irc::make_reply::inviting(channelname, nickname));
                    }
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
                // FIXME: implement
                std::string kick_comment;
                if (message.param_size() >= 3)
                {
                    kick_comment = message[2];
                }
                else
                {
                    // default message
                    kick_comment = "";
                }
                static_cast<void>(user), static_cast<void>(message);
                user.part_channel("kick reflected. please purchase super operator item.");
            }
        };
    }
}
