/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include "message.hpp"
#include "processor.hpp"
#include "reply.hpp"
#include "user.hpp"

#include <cstdlib>

namespace ft
{
    namespace irc
    {
        // Command: PASS
        // Parameters: <password>
        class processor_pass : public processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 1; }
            std::size_t get_max_params() const throw() { return 1; }

            bool is_registered_only() const throw() { return false; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                // FIXME: implement
                static_cast<void>(user), static_cast<void>(message);
            }
        };

        // Command: NICK
        // Parameters: <nickname> [ <hopcount> ]
        class processor_nick : public processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 1; }
            std::size_t get_max_params() const throw() { return 2; }

            bool is_registered_only() const throw() { return false; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                // FIXME: implement
                static_cast<void>(user), static_cast<void>(message);
            }
        };

        // Command: USER
        // Parameters: <username> <hostname> <servername> <realname>
        class processor_user : public processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 4; }
            std::size_t get_max_params() const throw() { return 4; }

            bool is_registered_only() const throw() { return false; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                // FIXME: implement
                static_cast<void>(user), static_cast<void>(message);
            }
        };

        // Command: OPER
        // Parameters: <user> <password>
        class processor_oper : public processor_base
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

        // Command: QUIT
        // Parameters: [<Quit message>]
        class processor_quit : public processor_base
        {
        public:
            std::size_t get_max_params() const throw() { return 1; }

            bool is_registered_only() const throw() { return false; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                // FIXME: implement
                static_cast<void>(user), static_cast<void>(message);
                user.send_message(ft::irc::make_error::no_privileges());
            }
        };
    }
}
