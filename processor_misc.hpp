/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include "message.hpp"
#include "processor.hpp"
#include "reply.hpp"
#include "server.hpp"
#include "user.hpp"

#include <cctype>
#include <cstddef>

namespace ft
{
    namespace irc
    {
        // Command: PING
        // Parameters: <token>
        class processor_ping : public ft::irc::processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 1; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                const std::string& token = message[0];
                user.send_message(ft::irc::make_reply::create("PONG") << token);
            }
        };

        // Command: KILL
        // Parameters: <nickname> <comment>
        class processor_kill : public ft::irc::processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 2; }
            std::size_t get_max_params() const throw() { return 2; }

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
        // Parameters: <sub command>
        class processor_cap : public ft::irc::processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 1; }

            bool is_registered_only() const throw() { return false; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                // ignore
                std::string sub_cmd = message[0];
                foreach (std::string::iterator, it, sub_cmd)
                {
                    *it = std::toupper(*it);
                }

                if (sub_cmd == "LS" || sub_cmd == "LIST")
                {
                    user.send_message(ft::irc::make_reply::create("CAP") << "*" << sub_cmd << "");
                }
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
                std::string target_name;
                if (message.param_size() == this->get_min_params())
                {
                    target_name = user.load_nick();
                }
                else
                {
                    target_name = message[0];
                }
                user.send_message(ft::irc::make_reply::end_of_who(target_name));
            }
        };

        // Command: WHOIS
        // Parameters: [<server>] <nickmask>[,<nickmask>[,...]]
        class processor_whois : public ft::irc::processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 1; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                // ignore
                user.send_message(ft::irc::make_reply::end_of_whois(message[0]));
            }
        };
    }
}
