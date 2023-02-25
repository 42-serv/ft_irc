/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "client.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>

namespace ft
{
    namespace irc
    {
        class channel : public ft::enable_shared_from_this<channel>
        {
        public:
            typedef int channel_mode;
            typedef ft::serv::fast_dictionary<std::string, ft::shared_ptr<client> >::type client_dictionary;

        private:
            std::string host;
            std::string serv;
            std::string pass;

            std::string channelname;
            channel_mode mode;
            client_dictionary clients;
            // FIXME channel operator info?

        public:
            channel(const std::string& host, const std::string& serv, const std::string& pass);
            ~channel();

        public:
            const std::string& get_host() const throw();
            const std::string& get_serv() const throw();
            const std::string& get_pass() const throw();

            const std::string& get_channelname() const throw();
            const channel_mode& get_mode() const throw();
            const client_dictionary& get_clients() const throw();

        private:
            channel(const channel&);
            channel& operator=(const channel&);
        };
    }
}
