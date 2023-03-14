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
#include "string_utils.hpp"
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

            // FIXME
            void on_read(ft::serv::event_layer& layer, ft::shared_ptr<void> arg)
            {
                ft::shared_ptr<ft::irc::message> message = ft::static_pointer_cast<ft::irc::message>(arg);
                // ft::irc::processor_dictionary::execute(*this->user, *message);

                ft::irc::make_reply_base::set_server_name(bot.get_servername());
                ft::irc::make_reply_base::set_user_name(bot.get_username());
                ft::irc::make_reply_base::set_user_nick(bot.get_nick());

                ft::serv::logger::debug("%s : %s", __PRETTY_FUNCTION__, message->to_pretty_string().c_str());
                static_cast<void>(layer);

                if (message->get_command() == "PRIVMSG")
                {
                    const std::string sender = ft::irc::string_utils::pick_nick(message->get_prefix());
                    const ft::irc::message payload = ft::irc::message("PRIVMSG") << sender << "THIS IS REPLY";

                    layer.post_write(ft::make_shared<ft::irc::message>(payload));
                    layer.post_flush();
                }
                else if (message->get_command() == "INVITE")
                {
                    // accept invite
                    // what if FT_IRC_CHANNEL_LIMIT_PER_USER ?? -> server 가 알아서 핸들
                    const ft::irc::message payload = ft::irc::message("JOIN") << (*message)[1];
                    layer.post_write(ft::make_shared<ft::irc::message>(payload));
                    layer.post_flush();
                }
                else
                {
                    return; // ignore
                }
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
