/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include "message.hpp"
#include "processor_base.hpp"
#include "user.hpp"

namespace ft
{
    namespace irc
    {
        // Command: KILL
        // Parameters: <nickname> <comment>
        class processor_kill : public processor_base
        {
        public:
            int get_min_params() const throw() { return 2; }
            int get_max_params() const throw() { return 3; }

            bool is_operator_only() const throw() { return true; }

            void execute(const ft::irc::user& user, const ft::irc::message& message) const
            {
                // FIXME: implement
                static_cast<void>(user), static_cast<void>(message);
            }
        };

        // Command: PING
        // Parameters: <server1> [<server2>]
        class processor_ping : public processor_base
        {
        public:
            int get_min_params() const throw() { return 1; }
            int get_max_params() const throw() { return 2; }

            void execute(const ft::irc::user& user, const ft::irc::message& message) const
            {
                // FIXME: implement
                static_cast<void>(user), static_cast<void>(message);
            }
        };

        // Command: PONG
        // Parameters: <daemon> [<daemon2>]
        class processor_pong : public processor_base
        {
        public:
            int get_min_params() const throw() { return 1; }
            int get_max_params() const throw() { return 2; }

            void execute(const ft::irc::user& user, const ft::irc::message& message) const
            {
                // FIXME: implement
                static_cast<void>(user), static_cast<void>(message);
            }
        };

        // Command: ERROR
        // Parameters: <error message>
        class processor_error : public processor_base
        {
        public:
            int get_min_params() const throw() { return 1; }
            int get_max_params() const throw() { return 1; }

            void execute(const ft::irc::user& user, const ft::irc::message& message) const
            {
                // FIXME: implement
                static_cast<void>(user), static_cast<void>(message);
            }
        };
    }
}