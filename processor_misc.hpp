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

        // Command: AWAY
        // Parameters: [message]
        class processor_away : public ft::irc::processor_base
        {
        public:
            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                if (message.param_size() == this->get_min_params())
                {
                    user.store_mode(user::USER_MODE_AWAY, false);
                    user.reset_away_message();
                    user.send_message(ft::irc::make_reply::unaway());
                }
                else
                {
                    user.store_mode(user::USER_MODE_AWAY, true);
                    user.store_away_message(message[0]);
                    user.send_message(ft::irc::make_reply::now_away());
                }
            }
        };

        // Command: CAP
        // Parameters: [capability]
        class processor_cap : public ft::irc::processor_base
        {
        public:
            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                // ignore
                static_cast<void>(user), static_cast<void>(message);
            }
        };

        // Command: WHO
        // Parameters: [<name> [<o>]]
        class processor_who : public ft::irc::processor_base
        {
        public:
            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                // ignore
                static_cast<void>(user), static_cast<void>(message);
            }
        };

        // Command: WHOIS
        // Parameters: [<server>] <nickmask>[,<nickmask>[,...]]
        class processor_whois : public ft::irc::processor_base
        {
        public:
            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                // ignore
                static_cast<void>(user), static_cast<void>(message);
            }
        };
    }
}
