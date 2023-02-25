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
        class processor_privmsg : public processor_base
        {
        public:
            int get_min_params() const throw() { return 2; }

            void execute(const ft::irc::user& user, const ft::irc::message& message)
            {
                // FIXME
            }
        };

        class processor_notice : public processor_base
        {
        public:
            int get_min_params() const throw() { return 2; }
            int get_max_params() const throw() { return 2; }

            void execute(const ft::irc::user& user, const ft::irc::message& message)
            {
                // FIXME
            }
        };
    }
}
