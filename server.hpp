/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "channel.hpp"
#include "user.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>

namespace ft
{
    namespace irc
    {
        class server : public ft::enable_shared_from_this<server>
        {
        public:
            typedef ft::serv::fast_dictionary<std::string, ft::shared_ptr<ft::irc::channel> >::type channel_dictionary;
            // typedef ft::serv::fast_dictionary<std::string, ft::shared_ptr<ft::irc::user> >::type user_dictionary;

        private:
            std::string pass;

            channel_dictionary channels;
            // user_dictionary users;

        public:
            server();
            ~server();

        public:
            ft::shared_ptr<ft::irc::channel> get_or_new_channel(const std::string& name);
            void remove_channel(const std::string& name);

        private:
            server(const server&);
            server& operator=(const server&);
        };
    }
}
