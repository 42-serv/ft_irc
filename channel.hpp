/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>
#include <thread/readwrite_lock.hpp>

#include <bitset>
#include <string>

namespace ft
{
    namespace irc
    {
        class user;

        class channel : public ft::enable_shared_from_this<channel>
        {
        public:
            enum channel_mode
            {
                CHANNEL_MODE_A,
                CHANNEL_MODE_B,
                CHANNEL_MODE_C,
                NUMBEROF_CHANNEL_MODE
            };
            typedef ft::serv::fast_dictionary<std::string, ft::shared_ptr<ft::irc::user> >::type user_dictionary;

        private:
            std::string name;
            std::bitset<NUMBEROF_CHANNEL_MODE> mode;
            user_dictionary users;
            // FIXME channel operator info?

        public:
            channel(const std::string& pass);
            ~channel();

        public:
            const std::string& get_name() const throw();
            bool get_mode(channel_mode index) const throw();
            const user_dictionary& get_users() const throw();

        private:
            channel(const channel&);
            channel& operator=(const channel&);
        };
    }
}
