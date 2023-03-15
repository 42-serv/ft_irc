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
#include "string_utils.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>

#include <cstdlib>
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

                ft::serv::logger::debug("%s : %s", __PRETTY_FUNCTION__, message->to_pretty_string().c_str());

                const std::string command = message->get_command();

                if (command == "PRIVMSG")
                {
                    const std::string sender = ft::irc::string_utils::pick_nick(message->get_prefix());
                    const std::string& receiver = (*message)[0];
                    const std::string& text = (*message)[1];

                    if (receiver.find_first_of(',') != std::string::npos)
                    {
                        // ignore multiple receiver message
                        return;
                    }

                    std::string target;
                    if (ft::irc::string_utils::is_valid_channelname(receiver))
                    {
                        // to channel
                        target = receiver;
                    }
                    else
                    {
                        // to user
                        target = sender;
                    }
                    const ft::irc::message payload = ft::irc::message("PRIVMSG") << target << text;
                    layer.post_write(ft::make_shared<ft::irc::message>(payload));
                }
                else if (command == "INVITE")
                {
                    const std::string sender = ft::irc::string_utils::pick_nick(message->get_prefix());
                    const std::string& channel = (*message)[1];

                    bot.add_inviter(channel, sender);

                    const ft::irc::message payload = ft::irc::message("JOIN") << channel;
                    layer.post_write(ft::make_shared<ft::irc::message>(payload));
                }
                else if (command == "KICK")
                {
                    const std::string& channel = (*message)[0];
                    const std::string& nick = (*message)[1];

                    if (this->bot.is_inviter(channel, nick))
                    {
                        this->bot.remove_inviter(channel);

                        const ft::irc::message payload = ft::irc::message("PART") << channel;
                        layer.post_write(ft::make_shared<ft::irc::message>(payload));
                    }
                }
                else if (command == "PART")
                {
                    const std::string nick = ft::irc::string_utils::pick_nick(message->get_prefix());
                    const std::string& channel = (*message)[0];

                    if (this->bot.is_inviter(channel, nick))
                    {
                        this->bot.remove_inviter(channel);

                        const ft::irc::message payload = ft::irc::message("PART") << channel;
                        layer.post_write(ft::make_shared<ft::irc::message>(payload));
                    }
                }
                else if (command == "QUIT")
                {
                    const std::string sender = ft::irc::string_utils::pick_nick(message->get_prefix());

                    const std::string channel_to_remove = this->bot.find_channels(sender);

                    const ft::irc::message payload = ft::irc::message("PART") << channel_to_remove;
                    layer.post_write(ft::make_shared<ft::irc::message>(payload));
                }
                else if (command == "NICK")
                {
                    const std::string old_nick = ft::irc::string_utils::pick_nick(message->get_prefix());
                    const std::string& new_nick = (*message)[0];

                    this->bot.update_nick(old_nick, new_nick);
                    return;
                }
                else
                {
                    // ignore
                    return;
                }
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

                std::exit(EXIT_SUCCESS);
            }
        };
    }
}
