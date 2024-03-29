/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>
#include <thread/readwrite_lock.hpp>

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
                REGISTER_STATE_COMPLETED,
                NUMBEROF_REGISTER_STATE
            };

            typedef ft::serv::dynamic_array<std::string>::type channel_list;
            typedef ft::serv::dynamic_array<ft::shared_ptr<channel>::element_type*>::type invite_list;

        private:
            ft::irc::server& server;
            ft::weak_ptr<ft::serv::event_layer> layer;
            std::string nick;
            std::string username;
            std::string hostname;
            std::string realname;
            channel_list channels;
            invite_list invites;
            std::bitset<NUMBEROF_USER_MODE> mode;
            std::bitset<NUMBEROF_REGISTER_STATE> registered_state;
            std::string away_message;
            std::string quit_message;
            mutable ft::readwrite_lock lock;

        public:
            user(ft::irc::server& server, const ft::shared_ptr<ft::serv::event_layer>& layer);
            ~user();

        public:
            ft::irc::server& get_server() const throw();

            const std::string& get_nick() const throw();
            void set_nick(const std::string& nick);
            std::string load_nick() const;
            bool change_nick(const std::string& nick);

            const std::string& get_username() const throw();
            void set_username(const std::string& username);

            const std::string& get_hostname() const throw();

            std::string make_full_name() const throw();

            const std::string& get_realname() const throw();
            void set_realname(const std::string& realname);

            bool get_mode(user_mode index) const throw();
            void set_mode(user_mode index, bool value) throw();
            bool load_mode(user_mode index) const throw();
            void store_mode(user_mode index, bool value) throw();
            std::string make_mode_string();

            bool get_register_state(register_state index) const throw();
            void set_register_state(register_state index, bool value) throw();
            bool is_registered() const throw();
            void register_to_server();
            void deregister_from_server();

            const std::string& get_away_message() const throw();
            void set_away_message(const std::string& away_message);
            std::string load_away_message() const;
            void store_away_message(const std::string& away_message);
            void reset_away_message() throw();

            const std::string& get_quit_message() const throw();
            void set_quit_message(const std::string& quit_message);
            std::string load_quit_message() const;
            void store_quit_message(const std::string& quit_message);
            void reset_quit_message() throw();

            channel_list channel_names_snapshot() const throw();
            channel_list::size_type channel_count() const throw();
            bool is_channel_member(const std::string& channelname) const throw();
            void join_channel(const std::string& channelname);
            void part_channel(const std::string& channelname);

            bool contains_invite(const ft::shared_ptr<channel>& channel) const;
            void add_invite(const ft::shared_ptr<channel>& channel);
            void remove_invite(const ft::shared_ptr<channel>& channel);

        public:
            void send_message(const ft::irc::message& message) const;
            void notify_message(const ft::irc::message& message) const;
            void exit_client() const;

        public:
            struct pred_equals_nick
            {
                const std::string& nick;

                pred_equals_nick(const std::string& nick) throw();

                bool operator()(const user& user) const throw();
                bool operator()(const user* user) const throw();
                bool operator()(const ft::shared_ptr<const user>& user) const throw();
            };

        private:
            user(const user&);
            user& operator=(const user&);
        };
    }
}
