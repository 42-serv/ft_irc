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

#include <cstdlib>
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
                ft::irc::message::param_vector channelnames = ft::irc::message::split(message[0], ',');

                ft::irc::message::param_vector keys;
                if (message.param_size() > 1)
                {
                    keys = ft::irc::message::split(message[1], ',');
                }
                static_cast<void>(keys); // TODO: secret channel

                foreach (ft::irc::message::param_vector::iterator, it, channelnames)
                {
                    const std::string& channelname = *it;

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
                    ft::irc::reply_numerics rpl = channel->enter_user(user.shared_from_this());
                    if (rpl == RPL_NONE)
                    {
                        user.join_channel(channel->get_name());
                    }
                    else
                    {
                        switch (rpl)
                        {
                        case ERR_NOSUCHCHANNEL:
                            user.send_message(ft::irc::make_error::no_such_channel(channelname));
                            break;

                        case ERR_CHANNELISFULL:
                            user.send_message(ft::irc::make_error::channel_is_full(channelname));
                            break;
                        case ERR_INVITEONLYCHAN:
                            user.send_message(ft::irc::make_error::invite_only_channel(channelname));
                            break;
                        case ERR_BANNEDFROMCHAN:
                            user.send_message(ft::irc::make_error::banned_from_channel(channelname));
                            break;
                        case ERR_BADCHANNELKEY:
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
                ft::irc::message::param_vector channelnames = ft::irc::message::split(message[0], ',');

                foreach (ft::irc::message::param_vector::iterator, it, channelnames)
                {
                    const std::string& channelname = *it;

                    if (user.is_channel_member(channelname))
                    {
                        user.part_channel(channelname);

                        ft::shared_ptr<ft::irc::channel> channel = server.find_channel(channelname);
                        if (channel)
                        {
                            channel->leave_user(user.shared_from_this());
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
                static_cast<void>(user), static_cast<void>(message);
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
                // FIXME: implement

                ft::irc::server& server = user.get_server();
                const std::string& channelname = message[0];

                ft::shared_ptr<ft::irc::channel> channel = server.find_channel(channelname);
                if (channel)
                {
                    if (message.param_size() == 1)
                    {
                        const std::string& topic = channel->get_topic();
                        if (!topic.empty())
                        {
                            user.send_message(ft::irc::make_reply::topic(channel->get_name(), channel->get_topic()));
                        }
                        else
                        {
                            user.send_message(ft::irc::make_reply::no_topic(channel->get_name()));
                        }
                    }
                    else
                    {
                        if (user.is_channel_member(channelname))
                        {
                            if (!channel->get_mode(ft::irc::channel::CHANNEL_MODE_TOPIC_LIMIT))  // FIXME: check is_channel_member && permission if channel mode require
                            {
                                const std::string& topic = message[1];
                                channel->set_topic(topic);
                                if (!topic.empty())
                                {
                                    channel->broadcast(ft::irc::message("NOTICE") >> "ft_irc"
                                                                                         << "channel " + channelname + "'s topic changed to " + channel->get_topic());
                                }
                                else
                                {
                                    channel->broadcast(ft::irc::message("NOTICE") >> "ft_irc"
                                                                                         << "channel " + channelname + "'s topic cleared");
                                }
                            }
                            else
                            {
                                user.send_message(ft::irc::make_error::channel_operator_privileges_needed(channelname));
                            }
                        }
                        else
                        {
                            user.send_message(ft::irc::make_error::not_on_channel(channel->get_name()));
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
                // FIXME: implement
                static_cast<void>(user), static_cast<void>(message);
            }
        };

        // Command: LIST
        // Parameters: [<channel>{,<channel>} [<server>]]
        class processor_list : public ft::irc::processor_base
        {
        public:
            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                // FIXME: implement
                static_cast<void>(user), static_cast<void>(message);
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
                // FIXME: implement
                static_cast<void>(user), static_cast<void>(message);
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
                static_cast<void>(user), static_cast<void>(message);
            }
        };
    }
}
