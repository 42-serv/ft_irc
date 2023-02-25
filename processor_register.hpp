/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include "message.hpp"
#include "user.hpp"

namespace ft
{
    namespace irc
    {
        // Command: PASS
        // Parameters: <password>
        class processor_pass : public processor_base
        {
        public:
            int get_min_params() const throw() { return 1; }
            int get_max_params() const throw() { return 1; }

            bool ft::irc::processor_base::is_registered_only() { return false; }

            void execute(const ft::irc::user& user, const ft::irc::message& message)
            {
                // FIXME
            }
        };

        // Command: NICK
        // Parameters: <nickname> [ <hopcount> ]
        class processor_nick : public processor_base
        {
        public:
            int get_min_params() const throw() { return 1; }
            int get_max_params() const throw() { return 2; }

            bool ft::irc::processor_base::is_registered_only() { return false; }

            void execute(const ft::irc::user& user, const ft::irc::message& message)
            {
                // FIXME
            }
        };

        // Command: USER
        // Parameters: <username> <hostname> <servername> <realname>
        class processor_user : public processor_base
        {
        public:
            int get_min_params() const throw() { return 4; }
            int get_max_params() const throw() { return 4; }

            bool ft::irc::processor_base::is_registered_only() { return false; }

            void execute(const ft::irc::user& user, const ft::irc::message& message)
            {
                // FIXME
            }
        };

        // Command: OPER
        // Parameters: <user> <password>
        class processor_oper : public processor_base
        {
        public:
            int get_min_params() const throw() { return 2; }
            int get_max_params() const throw() { return 2; }

            void execute(const ft::irc::user& user, const ft::irc::message& message)
            {
                // FIXME
            }
        };

        // Command: QUIT
        // Parameters: [<Quit message>]
        class processor_quit : public processor_base
        {
        public:
            int get_max_params() const throw() { return 1; }

            bool ft::irc::processor_base::is_registered_only() { return false; }

            void execute(const ft::irc::user& user, const ft::irc::message& message)
            {
                // FIXME
            }
        };

        // Command: SQUIT
        // Parameters: <server> <comment>
        class processor_squit : public processor_base // FIXME is this necessary???
        {
        public:
            int get_min_params() const throw() { return 2; }
            int get_max_params() const throw() { return 2; }

            bool ft::irc::processor_base::is_operator_only() { return true; }

            void execute(const ft::irc::user& user, const ft::irc::message& message)
            {
                // FIXME
            }
        };
    }
}
