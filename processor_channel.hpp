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
        // Command: JOIN
        // Parameters: <channel>{,<channel>} [<key>{,<key>}]
        class processor_join : public ft::irc::processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 1; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                // FIXME: implement
                static_cast<void>(user), static_cast<void>(message);
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
                // FIXME: implement
                static_cast<void>(user), static_cast<void>(message);
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
                static_cast<void>(user), static_cast<void>(message);
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
