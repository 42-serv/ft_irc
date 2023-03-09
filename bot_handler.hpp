/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include "bot.hpp"
#include "message_decoder.hpp"
#include "message_encoder.hpp"
#include "processor.hpp"
#include "reply.hpp"
#include "server.hpp"
#include "string_line_decoder.hpp"
#include "string_line_encoder.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>

#include <string>

namespace ft
{
    namespace irc
    {
        class bot_handler : public ft::serv::logic_adapter
        {
        public:
            ft::irc::bot& bot;

            bot_handler(ft::irc::bot& bot)
                : bot(bot)
            {
            }

        private:
            void on_active(ft::serv::event_layer& layer)
            {
                static_cast<void>(layer);
                ft::serv::logger::debug("%s", __PRETTY_FUNCTION__);

                layer.post_write(ft::make_shared<ft::irc::message>(ft::irc::message("PASS") << bot.get_pass()));
                layer.post_write(ft::make_shared<ft::irc::message>(ft::irc::message("NICK") << bot.get_nick()));
                layer.post_write(ft::make_shared<ft::irc::message>(ft::irc::message("USER") << bot.get_username() << bot.get_hostname() << bot.get_servername() << bot.get_realname()));
                layer.post_flush();
            }

            void on_read(ft::serv::event_layer& layer, ft::shared_ptr<void> arg)
            {
                ft::shared_ptr<ft::irc::message> message = ft::static_pointer_cast<ft::irc::message>(arg);
                // ft::irc::processor_dictionary::execute(*this->user, *message);

                ft::serv::logger::debug("%s : %s", __PRETTY_FUNCTION__, message->to_pretty_string().c_str());
                layer.post_write(message);
                layer.post_flush();
            }

            void on_read_complete(ft::serv::event_layer&)
            {
                ft::serv::logger::debug("%s", __PRETTY_FUNCTION__);
            }

            void on_error(ft::serv::event_layer& layer, ft::shared_ptr<const std::exception> eptr)
            {
                layer.post_disconnect();

                ft::serv::logger::debug("%s : %s", __PRETTY_FUNCTION__, eptr->what());
            }

            void on_inactive(ft::serv::event_layer&)
            {
                ft::serv::logger::debug("%s", __PRETTY_FUNCTION__);
            }
        };
    }
}
