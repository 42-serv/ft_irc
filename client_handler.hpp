/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include "message.hpp"
#include "server.hpp"
#include "user.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>

namespace ft
{
    namespace irc
    {
        class client_handler : public ft::serv::logic_adapter
        {
        public:
            ft::irc::server& server;
            ft::shared_ptr<ft::irc::user> user;

            client_handler(ft::irc::server& server)
                : server(server)
            {
            }

        private:
            void on_active(ft::serv::event_layer& layer)
            {
                std::cout << __PRETTY_FUNCTION__ << std::endl;
                user = ft::make_shared<ft::irc::user>(server, layer);
            }

            void on_read(ft::serv::event_layer& layer, ft::shared_ptr<void> arg)
            {
                ft::shared_ptr<ft::irc::message> message = ft::static_pointer_cast<ft::irc::message>(arg);
                std::cout << __PRETTY_FUNCTION__ << " : " << message->to_pretty_string() << std::endl;

                if (message->get_command() == "QUIT")
                {
                    layer.post_disconnect();
                }
                else
                {
                    layer.post_write(message);
                    layer.post_flush();
                }
            }

            void on_read_complete(ft::serv::event_layer&)
            {
                std::cout << __PRETTY_FUNCTION__ << std::endl;
            }

            void on_error(ft::serv::event_layer& layer, ft::shared_ptr<const std::exception> eptr)
            {
                std::cout << __PRETTY_FUNCTION__ << " : " << eptr->what() << std::endl;
                layer.post_disconnect();
            }

            void on_inactive(ft::serv::event_layer&)
            {
                std::cout << __PRETTY_FUNCTION__ << std::endl;
                user.reset();
            }
        };
    }
}
