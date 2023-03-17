/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "irc_constants.hpp"

#include "message.hpp"
#include "message_decoder.hpp"
#include "message_encoder.hpp"
#include "reply.hpp"
#include "string_line_decoder.hpp"
#include "string_line_encoder.hpp"
#include "string_utils.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>

#include <cstdlib>
#include <sstream>
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
            bot(const std::string& pass)
                : pass(pass),
                  nick("ircBot"),
                  username("user"),
                  hostname("local"),
                  servername("user"),
                  realname("42bot"),
                  inviters()
            {
            }

            ~bot()
            {
            }

        public:
            const std::string& get_pass() const throw()
            {
                return this->pass;
            }

            const std::string& get_nick() const throw()
            {
                return this->nick;
            }

            const std::string& get_username() const throw()
            {
                return this->username;
            }

            const std::string& get_hostname() const throw()
            {
                return this->hostname;
            }

            const std::string& get_servername() const throw()
            {
                return this->servername;
            }

            const std::string& get_realname() const throw()
            {
                return this->realname;
            }

            void add_inviter(const std::string& channel, const std::string& inviter)
            {
                this->inviters.insert(std::make_pair(channel, inviter));
            }

            bool is_inviter(const std::string& channel, const std::string& inviter) const
            {
                ft::irc::bot::inviter_dictionary::const_iterator it = this->inviters.find(channel);

                return it != this->inviters.end() && ft::irc::string_utils::is_same(it->second, inviter);
            }

            void remove_inviter(const std::string& channel) throw()
            {
                this->inviters.erase(channel);
            }

            const std::string find_channels(const std::string& sender)
            {
                std::vector<std::string> channels_to_remove;

                foreach (ft::irc::bot::inviter_dictionary::const_iterator, it, this->inviters)
                {
                    if (ft::irc::string_utils::is_same(it->second, sender))
                    {
                        channels_to_remove.push_back(it->first);
                    }
                }

                std::ostringstream oss;
                bool first = true;
                foreach (std::vector<std::string>::iterator, it, channels_to_remove)
                {
                    this->inviters.erase(*it);
                    if (first)
                    {
                        first = false;
                    }
                    else
                    {
                        oss << ',';
                    }
                    oss << *it;
                }
                return oss.str();
            }

            void update_nick(const std::string& old_nick, const std::string& new_nick)
            {
                foreach (ft::irc::bot::inviter_dictionary::iterator, it, this->inviters)
                {
                    if (ft::irc::string_utils::is_same(it->second, old_nick))
                    {
                        it->second = new_nick;
                    }
                }
            }

        public:
            void send_auth(ft::serv::event_layer& layer) const
            {
                layer.post_write(ft::make_shared<ft::irc::message>(ft::irc::message("PASS") << this->get_pass()));
                layer.post_write(ft::make_shared<ft::irc::message>(ft::irc::message("NICK") << this->get_nick()));
                layer.post_write(ft::make_shared<ft::irc::message>(ft::irc::message("USER") << this->get_username() << this->get_hostname() << this->get_servername() << this->get_realname()));
                layer.post_flush();
            }

            void on_packet(ft::serv::event_layer& layer, ft::irc::message& message)
            {
                const std::string command = message.get_command();

                if (command == "PRIVMSG")
                {
                    const std::string sender = ft::irc::string_utils::pick_nick(message.get_prefix());
                    const std::string& receiver = message[0];
                    const std::string& text = message[1];

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
                    const std::string sender = ft::irc::string_utils::pick_nick(message.get_prefix());
                    const std::string& channel = message[1];

                    this->add_inviter(channel, sender);

                    const ft::irc::message payload = ft::irc::message("JOIN") << channel;
                    layer.post_write(ft::make_shared<ft::irc::message>(payload));
                }
                else if (command == "KICK")
                {
                    const std::string& channel = message[0];
                    const std::string& nick = message[1];

                    if (this->is_inviter(channel, nick))
                    {
                        this->remove_inviter(channel);

                        const ft::irc::message payload = ft::irc::message("PART") << channel;
                        layer.post_write(ft::make_shared<ft::irc::message>(payload));
                    }
                    else if (std::string_utils::is_same(nick, this->get_nick()))
                    {
                        this->remove_inviter(channel);
                    }
                }
                else if (command == "PART")
                {
                    const std::string nick = ft::irc::string_utils::pick_nick(message.get_prefix());
                    const std::string& channel = message[0];

                    if (this->is_inviter(channel, nick))
                    {
                        this->remove_inviter(channel);

                        const ft::irc::message payload = ft::irc::message("PART") << channel;
                        layer.post_write(ft::make_shared<ft::irc::message>(payload));
                    }
                }
                else if (command == "QUIT")
                {
                    const std::string sender = ft::irc::string_utils::pick_nick(message.get_prefix());

                    const std::string channel_to_remove = this->find_channels(sender);

                    const ft::irc::message payload = ft::irc::message("PART") << channel_to_remove;
                    layer.post_write(ft::make_shared<ft::irc::message>(payload));
                }
                else if (command == "NICK")
                {
                    const std::string old_nick = ft::irc::string_utils::pick_nick(message.get_prefix());
                    const std::string& new_nick = message[0];

                    this->update_nick(old_nick, new_nick);
                    return;
                }
                else
                {
                    // ignore
                    return;
                }
                layer.post_flush();
            }

        private:
            bot(const bot&);
            bot& operator=(const bot&);
        };

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
                this->bot.send_auth(layer);
                ft::serv::logger::debug("%s", __PRETTY_FUNCTION__);
            }

            void on_read(ft::serv::event_layer& layer, ft::shared_ptr<void> arg)
            {
                ft::shared_ptr<ft::irc::message> message = ft::static_pointer_cast<ft::irc::message>(arg);

                this->bot.on_packet(layer, *message);
                ft::serv::logger::debug("%s : %s", __PRETTY_FUNCTION__, message->to_pretty_string().c_str());
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

        static ft::shared_ptr<ft::serv::event_channel_base> _make_client(ft::serv::ident_t ident, const std::string& host, int serv, void* arg)
        {
            ft::shared_ptr<ft::serv::event_channel_base> child = ft::make_shared<ft::serv::stream_channel>(ident, host, serv);
            child->add_last_handler(ft::make_shared<ft::irc::string_line_encoder>());
            child->add_last_handler(ft::make_shared<ft::irc::string_line_decoder>());
            child->add_last_handler(ft::make_shared<ft::irc::message_encoder>());
            child->add_last_handler(ft::make_shared<ft::irc::message_decoder>());
            child->add_last_handler(ft::make_shared<ft::irc::bot_handler>(*static_cast<ft::irc::bot*>(arg)));
            return child;
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        ft::serv::logger::warn("Usage: %s <host> <port> <pass>", argv[0]);
        return EXIT_FAILURE;
    }

    ft::shared_ptr<ft::serv::event_worker_group> child_group = ft::make_shared<ft::serv::event_worker_group>();
    child_group->put_worker(ft::make_shared<ft::serv::event_worker>());

    ft::irc::bot bot(argv[3]);
    ft::serv::bootstrap boot(child_group, child_group, null, &ft::irc::_make_client);
    try
    {
        if (!boot.start_client(argv[1], argv[2], &bot))
        {
            ft::serv::logger::error("bind failed. Is host \"%s\" or port \"%s\" wrong?", argv[1], argv[2]);
            return EXIT_FAILURE;
        }
        boot.set_success();
    }
    catch (const ft::serv::syscall_failed& e)
    {
        ft::serv::logger::error("bind failed. System error occured: %s", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
