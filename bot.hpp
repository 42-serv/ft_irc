/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>
#include <thread/readwrite_lock.hpp>

#include <string>

namespace ft
{
    namespace irc
    {
        class bot : public ft::enable_shared_from_this<bot>
        {
        public:
            typedef ft::serv::fast_dictionary<std::string, std::string>::type inviter_dictionary;

        private:
            std::string pass;

            std::string nick;

            std::string username;
            std::string hostname;
            std::string servername;
            std::string realname;
            inviter_dictionary inviters;

        public:
            bot(const std::string& pass); // FIXME
            ~bot();

        public:
            const std::string& get_pass() const throw();

            const std::string& get_nick() const throw();

            const std::string& get_username() const throw();
            const std::string& get_hostname() const throw();
            const std::string& get_servername() const throw();
            const std::string& get_realname() const throw();
            void add_inviter(const std::string& channel, const std::string& inviter) throw();
            const std::string find_channels(const std::string& inviter) const throw();
            bool check_is_inviter(const std::string& channel, const std::string& inviter);
            void remove_inviter(const std::string& channel);

        private:
            bot(const bot&);
            bot& operator=(const bot&);
        };
    }
}
