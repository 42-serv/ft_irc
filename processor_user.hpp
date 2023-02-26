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
        // Command: WHO
        // Parameters: [<name> [<o>]]
        class processor_who : public processor_base
        {
        public:
            void execute(const ft::irc::user& user, const ft::irc::message& message) const
            {
                // FIXME: implement
                static_cast<void>(user), static_cast<void>(message);
            }
        };

        // Command: WHOIS
        // Parameters: [<server>] <nickmask>[,<nickmask>[,...]]
        class processor_whois : public processor_base
        {
        public:
            int get_min_params() const throw() { return 1; }

            void execute(const ft::irc::user& user, const ft::irc::message& message) const
            {
                // FIXME: implement
                static_cast<void>(user), static_cast<void>(message);
            }
        };

        // Command: WHOWAS
        // Parameters: <nickname> [<count> [<server>]]
        class processor_whowas : public processor_base
        {
        public:
            int get_min_params() const throw() { return 1; }
            int get_max_params() const throw() { return 3; }

            void execute(const ft::irc::user& user, const ft::irc::message& message) const
            {
                // FIXME: implement
                static_cast<void>(user), static_cast<void>(message);
            }
        };
    }
}
