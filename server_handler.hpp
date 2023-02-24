/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include "client_handler.hpp"
#include "message_decoder.hpp"
#include "string_line_decoder.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>

#include "debug.hpp"

namespace ft
{
    namespace irc
    {
        class server_handler : public ft::serv::logic_adapter
        {
            void on_read(ft::serv::event_layer&, ft::shared_ptr<void> arg)
            {
                ft::shared_ptr<ft::serv::event_channel_base> child = ft::static_pointer_cast<ft::serv::event_channel_base>(arg);

                const ft::serv::ident_t child_ident = child->get_ident();
                ft::serv::socket_utils::set_tcp_nodelay(child_ident, true);
                ft::serv::socket_utils::set_socket_linger(child_ident, true, 5);

                child->add_last_handler(ft::make_shared<string_line_decoder>());
                child->add_last_handler(ft::make_shared<message_decoder>());
                child->add_first_handler(ft::make_shared<byte_buffer_dump_logger_handler>()); // NOTE: DEBUG
                child->add_last_handler(ft::make_shared<client_handler>());
            }
        };
    }
}