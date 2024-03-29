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
                ft::serv::logger::debug("%s", __PRETTY_FUNCTION__);

                this->user = ft::make_shared<ft::irc::user>(this->server, layer.shared_from_channel());
                if (this->server.get_pass().empty())
                {
                    this->user->set_register_state(ft::irc::user::REGISTER_STATE_PASS, true);
                }
            }

            void on_read(ft::serv::event_layer&, ft::shared_ptr<void> arg)
            {
                ft::shared_ptr<ft::irc::message> message = ft::static_pointer_cast<ft::irc::message>(arg);

                ft::serv::logger::debug("%s : %s", __PRETTY_FUNCTION__, message->to_pretty_string().c_str());

                ft::irc::processor_dictionary::execute(*this->user, *message);
            }

            void on_read_complete(ft::serv::event_layer&)
            {
                ft::serv::logger::debug("%s", __PRETTY_FUNCTION__);
            }

            void on_error(ft::serv::event_layer& layer, ft::shared_ptr<const std::exception> eptr)
            {
                ft::serv::logger::debug("%s : %s", __PRETTY_FUNCTION__, eptr->what());

                layer.post_disconnect();
            }

            void on_inactive(ft::serv::event_layer&)
            {
                ft::serv::logger::debug("%s", __PRETTY_FUNCTION__);

                this->user->deregister_from_server();
            }
        };
    }
}
