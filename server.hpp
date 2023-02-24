/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <smart_ptr/smart_ptr.hpp>

namespace ft
{
    namespace irc
    {
        class server : public ft::enable_shared_from_this<server>
        {
        public:
            typedef ft::fast_dictionary<std::string, ft::shared_ptr<channel> >::type channel_dictionary;
            typedef ft::fast_dictionary<std::string, ft::shared_ptr<client> >::type client_dictionary;

        private:
            std::string pass;
            channel_dictionary channels;
            client_dictionary clients;

        public:
            server(const std::string& host, const std::string& serv, const std::string& pass);
            ~server();

        private:
            server(const server&);
            server& operator=(const server&);
        };
    }
}
