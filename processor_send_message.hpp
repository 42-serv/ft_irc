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
        // Command: PRIVMSG
        // Parameters: <receiver>{,<receiver>} <text to be sent>
        class processor_privmsg : public ft::irc::processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 2; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                // FIXME: implement
                static_cast<void>(user), static_cast<void>(message);
            }
        };

        // Command: NOTICE
        // Parameters: <nickname> <text>
        class processor_notice : public ft::irc::processor_base
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
    }
}
