/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <smart_ptr/smart_ptr.hpp>

namespace ft
{
    namespace irc
    {
        class client : public ft::enable_shared_from_this<client>
        {
        public:
            typedef ft::fast_dictionary<std::string, std::string>::type channel_name_dictionary; // FIXME fast_set???

        private:
            std::string nick;
            std::string username;
            std::string hostname;
            std::string servername;
            std::string realname;
            channel_name_dictionary channels;

        public:
            client(const std::string& nick, const std::string& username, const std::string& hostname, const std::string& servername, const std::string realname);
            ~client();

        private:
            client(const client&);
            client& operator=(const client&);
        };
    }
}
