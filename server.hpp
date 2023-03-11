/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include "reply.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>
#include <thread/readwrite_lock.hpp>

#include <string>

namespace ft
{
    namespace irc
    {
        class message;

        class channel;

        class user;

        class server : public ft::enable_shared_from_this<server>
        {
        public:
            typedef ft::serv::fast_dictionary<std::string, ft::shared_ptr<ft::irc::channel> >::type channel_dictionary;
            typedef ft::serv::unique_set<std::string>::type nick_set;
            typedef ft::serv::dynamic_array<ft::shared_ptr<ft::irc::user> >::type user_list;

        private:
            std::string pass;
            channel_dictionary channels;
            nick_set nicks;
            user_list users;
            mutable ft::readwrite_lock lock;

        public:
            server(const std::string& pass);
            ~server();

        public:
            std::string make_full_name() const throw();
            const std::string& get_pass() const throw();

            ft::shared_ptr<ft::irc::channel> find_channel(const std::string& name) const throw();
            ft::shared_ptr<ft::irc::channel> ensure_channel(const std::string& name);
            void remove_channel(const std::string& name);

            bool hold_nick(const std::string& nick);
            void release_nick(const std::string& nick) throw();

            ft::shared_ptr<ft::irc::user> find_user(const std::string& name) const throw();
            void register_user(const ft::shared_ptr<ft::irc::user>& user);
            void deregister_user(const ft::shared_ptr<ft::irc::user>& user);

        public:
            void broadcast_all(const ft::irc::message& message, ft::shared_ptr<const ft::irc::user> except = ft::shared_ptr<ft::irc::user>()) const;

        public:
            ft::irc::reply_numerics check_signature(const std::string& user, const std::string& pass) const;

        private:
            server(const server&);
            server& operator=(const server&);
        };
    }
}
