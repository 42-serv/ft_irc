/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include "reply.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>
#include <thread/readwrite_lock.hpp>

#include <algorithm>
#include <bitset>
#include <cstddef>
#include <string>

namespace ft
{
    namespace irc
    {
        class message;

        class server;

        class user;

        class channel : public ft::enable_shared_from_this<channel>
        {
        public:
            enum channel_mode
            {
                CHANNEL_MODE_PRIVATE,
                CHANNEL_MODE_SECRET,
                CHANNEL_MODE_MODERATED,
                CHANNEL_MODE_TOPIC_LIMIT,
                CHANNEL_MODE_INVITE_ONLY,
                CHANNEL_MODE_NO_PRIVMSG,
                CHANNEL_MODE_KEY,
                CHANNEL_MODE_BAN,
                CHANNEL_MODE_LIMIT,
                NUMBEROF_CHANNEL_MODE
            };

            struct member
            {
                enum member_mode
                {
                    MEMBER_MODE_OWNER,
                    MEMBER_MODE_OPERATOR,
                    MEMBER_MODE_VOICE,
                    NUMBEROF_MEMBER_MODE
                };

                explicit member(const ft::shared_ptr<ft::irc::user>& user);

                ft::shared_ptr<ft::irc::user> user;
                std::bitset<NUMBEROF_MEMBER_MODE> mode;

                bool operator==(const ft::shared_ptr<const ft::irc::user>& that) const throw();
                bool operator==(const ft::irc::user& that) const throw();
            };

            typedef ft::serv::dynamic_array<member>::type member_list;

        private:
            ft::irc::server& server;
            std::string name;
            std::string topic;
            std::bitset<NUMBEROF_CHANNEL_MODE> mode;
            std::size_t limit;
            std::string key;
            member_list members;

            mutable ft::readwrite_lock lock;
            bool invalidated;

        public:
            channel(ft::irc::server& server, const std::string& pass);
            ~channel();

        public:
            const std::string& get_name() const throw();

            const std::string& get_topic() const throw();
            void set_topic(const std::string& topic);
            std::string load_topic() const;
            void store_topic(const std::string& topic);

            bool get_mode(channel_mode index) const throw();
            void set_mode(channel_mode index, bool value) throw();
            bool load_mode(channel_mode index) const throw();
            void store_mode(channel_mode index, bool value) throw();
            std::string make_mode_string(std::string* param);

            std::size_t get_limit() const throw();
            void set_limit(std::size_t limit) throw();
            std::size_t load_limit() const throw();
            void store_limit(std::size_t limit) throw();

            const std::string& get_key() const throw();
            void set_key(const std::string& key);
            std::string load_key() const;
            void store_key(const std::string& key);

        public:
            ft::irc::reply_numerics enter_user(ft::irc::user& user, const std::string& key);
            void leave_user(const ft::irc::user& user);

            void send_names(const ft::irc::user& user) const throw();
            ft::irc::message make_list_packet(bool force) const throw();

        public:
            void broadcast(const ft::irc::message& message, const ft::irc::user* except = null) const;
            void broadcast_unique(const ft::irc::message& message, ft::serv::unique_set<ft::shared_ptr<const ft::irc::user> >::type& unique_set) const;

        public:
            bool is_channel_operator(const ft::irc::user& user) const throw();
            bool is_channel_speaker(const ft::irc::user& user) const throw();
            bool is_banned(const ft::irc::user& user) const throw();

        private:
            channel(const channel&);
            channel& operator=(const channel&);
        };
    }
}
