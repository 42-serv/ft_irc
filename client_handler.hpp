/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include "message.hpp"
#include "processor.hpp"
#include "reply.hpp"
#include "server.hpp"
#include "user.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>

#include <string>

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
                this->user = ft::make_shared<ft::irc::user>(server, layer.shared_from_channel());

                ft::serv::logger::debug(__PRETTY_FUNCTION__);
            }

            void on_read(ft::serv::event_layer&, ft::shared_ptr<void> arg)
            {
                ft::shared_ptr<ft::irc::message> message = ft::static_pointer_cast<ft::irc::message>(arg);
                if (!this->user)
                {
                    ft::serv::logger::debug(__PRETTY_FUNCTION__ + (" : " + message->to_pretty_string()));
                    return;
                }
                ft::irc::processor_dictionary::execute(*this->user, *message);

                // ft::serv::logger::debug(__PRETTY_FUNCTION__ + (" : " + message->to_pretty_string()));
            }

            void on_read_complete(ft::serv::event_layer&)
            {
                ft::serv::logger::debug(__PRETTY_FUNCTION__);
            }

            void on_error(ft::serv::event_layer& layer, ft::shared_ptr<const std::exception> eptr)
            {
                layer.post_disconnect();

                ft::serv::logger::debug(__PRETTY_FUNCTION__ + (" : " + std::string(eptr->what())));
            }

            void on_inactive(ft::serv::event_layer&)
            {
                if (this->user)
                {
                    this->user->deregister_from_server();
                    this->user.reset();
                }

                ft::serv::logger::debug(__PRETTY_FUNCTION__);
            }
        };
    }
}
