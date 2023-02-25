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
        class processor_base
        {
        public:
            virtual ~processor_base();
            virtual int get_min_params();
            virtual int get_max_params();
            virtual bool is_registered_only();
            virtual bool is_operator_only();
            virtual void execute(const ft::irc::user& user, const ft::irc::message& msg) = 0;
        };
    }
}
