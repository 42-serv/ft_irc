/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>

#include <bitset>
#include <string>

namespace ft
{
    namespace irc
    {
        class message;

        class server;

        class channel;

        class user : public ft::enable_shared_from_this<user>
        {
        public:
            enum user_mode
            {
                USER_MODE_OPERATOR,
                USER_MODE_INVISIBLE,
                USER_MODE_AWAY,
                NUMBEROF_USER_MODE
            };

            enum register_state
            {
                REGISTER_STATE_PASS,
                REGISTER_STATE_NICK,
                REGISTER_STATE_USER,
                NUMBEROF_REGISTER_STATE

            };

            typedef ft::serv::dynamic_array<std::string>::type channel_list;

        private:
            ft::irc::server& server;
            ft::serv::event_layer& layer;
            std::string nick;
            std::string username;
            std::string hostname;
            std::string servername;
            std::string realname;
            channel_list channels;
            std::bitset<NUMBEROF_USER_MODE> mode;
            std::bitset<NUMBEROF_REGISTER_STATE> state;

        public:
            user(ft::irc::server& server, ft::serv::event_layer& layer);
            ~user();

        public:
            const std::string& get_nick() const throw();
            const std::string& get_username() const throw();
            const std::string& get_hostname() const throw();
            const std::string& get_servername() const throw();
            const std::string& get_realname() const throw();
            const channel_list& get_channels() const throw();
            bool get_mode(user_mode index) const throw();

            bool is_member_of(const std::string& channelname) const throw();
            channel_list::size_type get_channel_number() const throw();

            void set_nick(const std::string& nick);
            void set_mode(user_mode index, bool value);
            void join_channel(const std::string& channelname);
            void part_channel(const std::string& channelname);
            void finalize();

        public:
            void send_message(const ft::irc::message& message) const;

        private:
            user(const user&);
            user& operator=(const user&);
        };
    }
}
