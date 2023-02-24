/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include "message.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>

namespace ft
{
    namespace irc
    {
        class client_handler : public ft::serv::logic_adapter
        {
            void on_active(ft::serv::event_layer&)
            {
                std::cout << __PRETTY_FUNCTION__ << std::endl;
            }

            void on_read(ft::serv::event_layer& layer, ft::shared_ptr<void> arg)
            {
                ft::shared_ptr<message> msg = ft::static_pointer_cast<message>(arg);
                std::cout << __PRETTY_FUNCTION__ << " : " << msg->to_pretty_string() << std::endl;
                if (msg->get_command() == "QUIT")
                {
                    layer.post_disconnect();
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
            }
        };
    }
}