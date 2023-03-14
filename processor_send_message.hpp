/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include "channel.hpp"
#include "message.hpp"
#include "processor.hpp"
#include "reply.hpp"
#include "server.hpp"
#include "string_utils.hpp"
#include "user.hpp"

#include <cstddef>

namespace ft
{
    namespace irc
    {
        class processor_message_base : public ft::irc::processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 2; }
            virtual bool is_notice() const throw() = 0;

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                if (message[0].empty())
                {
                    user.send_message(ft::irc::make_error::no_recipient(message.get_command()));
                    return;
                }
                if (message[1].empty())
                {
                    user.send_message(ft::irc::make_error::no_text_to_send());
                    return;
                }

                ft::irc::server& server = user.get_server();
                const ft::irc::message::param_vector receivers = ft::irc::message::split(message[0], ',');

                foreach (ft::irc::message::param_vector::const_iterator, it, receivers)
                {
                    const std::string& receiver = *it;

                    ft::irc::message payload = ft::irc::make_reply::replicate(message);
                    payload[0] = receiver;

                    if (ft::irc::string_utils::is_valid_channelname(receiver))
                    {
                        ft::shared_ptr<ft::irc::channel> channel = server.find_channel(receiver);
                        if (channel)
                        {
                            if (channel->load_mode(ft::irc::channel::CHANNEL_MODE_NO_PRIVMSG) && !user.is_channel_member(receiver))
                            {
                                user.send_message(ft::irc::make_error::cannot_send_to_channel(receiver));
                                continue;
                            }

                            if (channel->load_mode(ft::irc::channel::CHANNEL_MODE_MODERATED) && !channel->is_channel_speaker(user))
                            {
                                user.send_message(ft::irc::make_error::cannot_send_to_channel(receiver));
                                continue;
                            }

                            channel->broadcast(payload, &user);
                        }
                        else
                        {
                            user.send_message(ft::irc::make_error::no_such_nickname(receiver));
                        }
                    }
                    else
                    {
                        ft::shared_ptr<ft::irc::user> target = server.find_user(receiver);
                        if (target)
                        {
                            target->send_message(payload);

                            if (!this->is_notice() && target->load_mode(user::USER_MODE_AWAY))
                            {
                                user.send_message(ft::irc::make_reply::away(receiver, target->load_away_message()));
                            }
                        }
                        else
                        {
                            user.send_message(ft::irc::make_error::no_such_nickname(receiver));
                        }
                    }
                }
            }
        };

        // Command: PRIVMSG
        // Parameters: <receiver>{,<receiver>} <text to be sent>
        class processor_privmsg : public ft::irc::processor_message_base
        {
        public:
            std::size_t get_min_params() const throw() { return 2; }
            bool is_notice() const throw() { return false; }
        };

        // Command: NOTICE
        // Parameters: <nickname> <text>
        class processor_notice : public ft::irc::processor_message_base
        {
        public:
            std::size_t get_min_params() const throw() { return 2; }
            bool is_notice() const throw() { return true; }
        };
    }
}
