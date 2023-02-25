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
            typedef ft::dynamic_array<std::string>::type channel_list;
            typedef int user_mode;

        private:
            std::string nick;
            std::string username;
            std::string hostname;
            std::string servername;
            std::string realname;
            channel_list channels;
            user_mode mode;

        public:
            // FIXME 3 steps needed to register: PASS, NICK, USER commands.
            client(const std::string& nick, const std::string& username, const std::string& hostname, const std::string& servername, const std::string& realname); // FIXME if directly connected client, ignore  hostname, servername ??
            ~client();

        public:
            const std::string& get_nick() const throw();
            const std::string& get_username() const throw();
            const std::string& get_hostname() const throw();
            const std::string& get_servername() const throw();
            const std::string& get_realname() const throw();
            const channel_list& get_channels() const throw();
            const user_mode& get_mode() const throw();

            bool is_member_of(const std::string& channelname) const throw();
            channel_list::size_type get_channel_number() const throw();

            void set_nick(const std::string& nick);
            void set_mode(const std::string& nick);
            void join_channel(const std::string& channelname);
            void part_channel(const std::string& channelname);

        private:
            client(const client&);
            client& operator=(const client&);
        };
    }
}
