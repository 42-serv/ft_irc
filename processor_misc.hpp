/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include "message.hpp"
#include "processor.hpp"
#include "reply.hpp"
#include "server.hpp"
#include "user.hpp"

#include <cstddef>

namespace ft
{
    namespace irc
    {
        // Command: KILL
        // Parameters: <nickname> <comment>
        class processor_kill : public ft::irc::processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 2; }
            std::size_t get_max_params() const throw() { return 3; }

            bool is_operator_only() const throw() { return true; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                static_cast<void>(message[1]); // comment
                const std::string& target_nick = message[0];
                ft::irc::server& server = user.get_server();

                const ft::shared_ptr<ft::irc::user>& target = server.find_user(target_nick);
                if (!target)
                {
                    user.send_message(ft::irc::make_error::no_such_nickname(target_nick));
                    return;
                }

                target->notify_message(ft::irc::make_reply::replicate(message));
                target->exit_client();
            }
        };

        // Command: PING
        // Parameters: <server1> [<server2>]
        class processor_ping : public ft::irc::processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 1; }
            std::size_t get_max_params() const throw() { return 2; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                // FIXME: implement
                static_cast<void>(user), static_cast<void>(message);
            }
        };

        // Command: PONG
        // Parameters: <daemon> [<daemon2>]
        class processor_pong : public ft::irc::processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 1; }
            std::size_t get_max_params() const throw() { return 2; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                // FIXME: implement
                static_cast<void>(user), static_cast<void>(message);
            }
        };

        // Command: ERROR
        // Parameters: <error message>
        class processor_error : public ft::irc::processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 1; }
            std::size_t get_max_params() const throw() { return 1; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                // FIXME: implement
                static_cast<void>(user), static_cast<void>(message);
            }
        };
    }
}
