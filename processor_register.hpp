/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include "message.hpp"
#include "processor.hpp"
#include "reply.hpp"
#include "server.hpp"
#include "string_utils.hpp"
#include "user.hpp"

#include <cstddef>
#include <string>

namespace ft
{
    namespace irc
    {
        // Command: PASS
        // Parameters: <password>
        class processor_pass : public ft::irc::processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 1; }
            std::size_t get_max_params() const throw() { return 1; }

            bool is_registered_only() const throw() { return false; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                if (user.is_registered())
                {
                    user.send_message(ft::irc::make_error::already_registered());
                    return;
                }

                const ft::irc::server& server = user.get_server();
                user.set_register_state(ft::irc::user::REGISTER_STATE_PASS, server.get_pass().empty() || message[0] == server.get_pass());
            }
        };

        // Command: NICK
        // Parameters: <nickname> [ <hopcount> ]
        class processor_nick : public ft::irc::processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 1; }
            std::size_t get_max_params() const throw() { return 2; }

            bool is_registered_only() const throw() { return false; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                if (!user.get_register_state(ft::irc::user::REGISTER_STATE_PASS))
                {
                    return;
                }

                const std::string& nick = message[0];

                if (nick.empty())
                {
                    user.send_message(ft::irc::make_error::no_nickname_given());
                    return;
                }

                if (!ft::irc::string_utils::is_valid_nick(nick))
                {
                    user.send_message(ft::irc::make_error::erroneous_nickname(nick));
                    return;
                }

                if (!user.change_nick(nick))
                {
                    user.send_message(ft::irc::make_error::nickname_in_use(nick));
                    return;
                }

                user.set_register_state(ft::irc::user::REGISTER_STATE_NICK, true);
                user.notify_message(ft::irc::make_reply::replicate(message));

                if (!user.is_registered() && user.get_register_state(ft::irc::user::REGISTER_STATE_USER))
                {
                    // first
                    user.register_to_server();
                }
            }
        };

        // Command: USER
        // Parameters: <username> <hostname> <servername> <realname>
        class processor_user : public ft::irc::processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 4; }
            std::size_t get_max_params() const throw() { return 4; }

            bool is_registered_only() const throw() { return false; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                if (!user.get_register_state(ft::irc::user::REGISTER_STATE_PASS))
                {
                    return;
                }

                if (user.is_registered())
                {
                    user.send_message(ft::irc::make_error::already_registered());
                    return;
                }

                user.set_username(message[0]);
                static_cast<void>(message[1]); // ignore user input hostname
                static_cast<void>(message[2]); // ignore user input servername
                user.set_realname(message[3]);
                user.set_register_state(ft::irc::user::REGISTER_STATE_USER, true);

                if (!user.is_registered() && user.get_register_state(ft::irc::user::REGISTER_STATE_NICK))
                {
                    // first
                    user.register_to_server();
                }
            }
        };

        // Command: OPER
        // Parameters: <user> <password>
        class processor_oper : public ft::irc::processor_base
        {
        public:
            std::size_t get_min_params() const throw() { return 2; }
            std::size_t get_max_params() const throw() { return 2; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                if (!user.get_register_state(ft::irc::user::REGISTER_STATE_PASS))
                {
                    return;
                }

                ft::irc::server& server = user.get_server();

                const std::string sign_user = message[0];
                const std::string sign_pass = message[1];

                ft::irc::reply_numerics rpl = server.check_signature(sign_user, sign_pass);
                if (rpl == ft::irc::RPL_NONE)
                {
                    user.set_mode(user::USER_MODE_OPERATOR, true);
                    user.send_message(ft::irc::make_reply::now_operator());
                    server.broadcast_all(ft::irc::make_reply::create("NOTICE") << '*' << user.make_full_name() + " is now opped.");
                }
                else
                {
                    switch (rpl)
                    {
                    case ft::irc::ERR_PASSWDMISMATCH:
                        user.send_message(ft::irc::make_error::password_mismatch());
                        break;
                    case ft::irc::ERR_NOOPERHOST:
                        user.send_message(ft::irc::make_error::no_operator_host());
                        break;

                    default:
                        assert(false);
                    }
                }
            }
        };

        // Command: QUIT
        // Parameters: [<Quit message>]
        class processor_quit : public ft::irc::processor_base
        {
        public:
            std::size_t get_max_params() const throw() { return 1; }

            bool is_registered_only() const throw() { return false; }

            void execute(ft::irc::user& user, const ft::irc::message& message) const
            {
                if (message.param_size() == this->get_min_params())
                {
                    user.reset_quit_message();
                }
                else
                {
                    user.store_quit_message(message[0]);
                }
                user.exit_client();
            }
        };
    }
}
