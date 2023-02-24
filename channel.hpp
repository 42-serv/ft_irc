/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <smart_ptr/smart_ptr.hpp>

namespace ft
{
    namespace irc
    {
        class channel : public ft::enable_shared_from_this<channel>
        {
        public:
            typedef int channel_mode;
            typedef ft::fast_dictionary<std::string, ft::shared_ptr<client> >::type client_dictionary;

        private:
            std::string channelname;
            channel_mode mode;
            client_dictionary clients;
            // FIXME channel operator info?

        public:
            channel(const std::string& host, const std::string& serv, const std::string& pass);
            ~channel();

        private:
            channel(const channel&);
            channel& operator=(const channel&);
        };
    }
}