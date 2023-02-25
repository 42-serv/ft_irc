/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "processor_base.hpp"

namespace ft
{
    namespace irc
    {
        class processor_join : public processor_base
        {
        public:
            int get_min_params() const throw() { return 1; }

            void execute(const ft::irc::user& user, const ft::irc::message& message)
            {
                // FIXME
            }
        };

        class processor_part : public processor_base
        {
        public:
            int get_min_params() const throw() { return 1; }

            void execute(const ft::irc::user& user, const ft::irc::message& message)
            {
                // FIXME
            }
        };

        class processor_mode : public processor_base
        {
        public:
            int get_min_params() const throw() { return 1; } // FIXME two commands: channel mode vs user mode

            void execute(const ft::irc::user& user, const ft::irc::message& message)
            {
                // FIXME
            }
        };

        class processor_topic : public processor_base
        {
        public:
            int get_min_params() const throw() { return 1; }
            int get_max_params() const throw() { return 2; }

            void execute(const ft::irc::user& user, const ft::irc::message& message)
            {
                // FIXME
            }
        };

        class processor_names : public processor_base
        {
        public:
            void execute(const ft::irc::user& user, const ft::irc::message& message)
            {
                // FIXME
            }
        };

        class processor_list : public processor_base
        {
        public:
            void execute(const ft::irc::user& user, const ft::irc::message& message)
            {
                // FIXME
            }
        };

        class processor_invite : public processor_base
        {
        public:
            int get_min_params() const throw() { return 2; }
            int get_max_params() const throw() { return 2; }

            void execute(const ft::irc::user& user, const ft::irc::message& message)
            {
                // FIXME
            }
        };

        class processor_kick : public processor_base
        {
        public:
            int get_min_params() const throw() { return 2; }
            int get_max_params() const throw() { return 3; }

            void execute(const ft::irc::user& user, const ft::irc::message& message)
            {
                // FIXME
            }
        };
    }
}
