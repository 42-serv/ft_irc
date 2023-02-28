/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include "channel.hpp"
#include "message.hpp"
#include "processor.hpp"
#include "reply.hpp"
#include "server.hpp"
#include "user.hpp"

#include <cstdlib>

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
                ft::irc::server& server = user.get_server();
                const ft::irc::message::param_vector receivers = ft::irc::message::split(message[0], ',');

                foreach (ft::irc::message::param_vector::const_iterator, it, receivers)
                {
                    const std::string& receiver = *it;

                    // TODO: check channel if '#' or '&'
                    // TODO: check pattern?
                    // TODO: use `if (this->is_notice());`?
                    ft::shared_ptr<ft::irc::channel> channel = server.find_channel(receiver);
                    if (channel)
                    {
                        channel->broadcast(ft::irc::make_reply::replicate(message), user.shared_from_this());
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
