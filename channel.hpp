/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>
#include <thread/readwrite_lock.hpp>

#include <algorithm>
#include <bitset>
#include <string>

namespace ft
{
    namespace irc
    {
        class message;

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
                // CHANNEL_MODE_KEY,
                // CHANNEL_MODE_BAN,
                // CHANNEL_MODE_LIMIT,
                NUMBEROF_CHANNEL_MODE
            };
            enum member_mode
            {
                MEMBER_MODE_OWNER,
                MEMBER_MODE_OPERATOR,
                MEMBER_MODE_VOICE,
                NUMBEROF_MEMBER_MODE
            };
            typedef ft::serv::fast_dictionary<std::string, std::pair<ft::shared_ptr<ft::irc::user>, member_mode> >::type user_dictionary;

        private:
            std::string name;
            std::bitset<NUMBEROF_CHANNEL_MODE> mode;
            user_dictionary users;
            ft::readwrite_lock lock;
            // FIXME channel operator info?

        public:
            channel(const std::string& pass);
            ~channel();

        public:
            const std::string& get_name() const throw();
            bool get_mode(channel_mode index) const throw();
            const user_dictionary& get_users() const throw();

        public:
            void broadcast(const ft::irc::message& message) const;

        private:
            channel(const channel&);
            channel& operator=(const channel&);
        };
    }
}
