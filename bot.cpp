/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "irc_constants.hpp"

#include "libserv/serv_types.hpp"
#include "message.hpp"
#include "message_decoder.hpp"
#include "message_encoder.hpp"
#include "reply.hpp"
#include "string_line_decoder.hpp"
#include "string_line_encoder.hpp"
#include "string_utils.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>

#include <cctype>
#include <csignal>
#include <cstdlib>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

namespace ft
{
    namespace irc
    {
        class bot : public ft::enable_shared_from_this<bot>
        {
        public:
            typedef ft::serv::fast_dictionary<std::string, std::string>::type owner_dictionary;
            typedef ft::serv::fast_dictionary<std::string, std::vector<std::string> >::type names_dictionary;

        private:
            std::string pass;

            std::string nick;

            std::string username;
            std::string hostname;
            std::string servername;
            std::string realname;

            std::string path;

            owner_dictionary channel_owners;
            names_dictionary names_map;

        public:
            bot(const std::string& pass, const std::string& nick, const std::string& username, const std::string& realname, const std::string& path)
                : pass(pass),
                  nick(nick),
                  username(username),
                  hostname("*"),
                  servername("*"),
                  realname(realname),
                  path(path),
                  channel_owners(),
                  names_map()
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

            const std::string& get_owner(const std::string& channel) const throw()
            {
                ft::irc::bot::owner_dictionary::const_iterator it = this->channel_owners.find(channel);

                if (it != this->channel_owners.end())
                {
                    return it->second;
                }

                static std::string empty;
                return empty;
            }

            void set_owner(const std::string& channel, const std::string& owner_nick)
            {
                this->channel_owners.insert(std::make_pair(channel, owner_nick));
            }

            bool is_owner(const std::string& channel, const std::string& nick) const
            {
                const std::string& owner_nick = this->get_owner(channel);
                return !owner_nick.empty() && ft::irc::string_utils::is_same(owner_nick, nick);
            }

            void leave_channel(const std::string& channel) throw()
            {
                this->channel_owners.erase(channel);
            }

            std::vector<std::string> find_own_channels(const std::string& owner_nick)
            {
                std::vector<std::string> channel_list;

                foreach (ft::irc::bot::owner_dictionary::const_iterator, it, this->channel_owners)
                {
                    if (ft::irc::string_utils::is_same(it->second, owner_nick))
                    {
                        channel_list.push_back(it->first);
                    }
                }

                return channel_list;
            }

            void update_nick(const std::string& old_nick, const std::string& new_nick)
            {
                foreach (ft::irc::bot::owner_dictionary::iterator, it, this->channel_owners)
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

            void send_packet(ft::serv::event_layer& layer, const ft::irc::message& msg) const
            {
                layer.post_write(ft::make_shared<ft::irc::message>(msg));
                layer.post_flush();
            }

            void on_privmsg(ft::serv::event_layer& layer, ft::irc::message& message)
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

                int fildes[2];
                if (::pipe(fildes) < 0)
                {
                    std::exit(EXIT_FAILURE);
                }

                ::pid_t pid = ::fork();
                if (pid < 0)
                {
                    std::exit(EXIT_FAILURE);
                }
                if (pid == 0)
                {
                    // child
                    ::close(fildes[STDIN_FILENO]);
                    if (::dup2(fildes[STDOUT_FILENO], STDOUT_FILENO) < 0)
                    {
                        std::exit(EXIT_FAILURE);
                    }
                    ::close(fildes[STDOUT_FILENO]);
                    if (::pipe(fildes) < 0)
                    {
                        std::exit(EXIT_FAILURE);
                    }
                    if (::dup2(fildes[STDIN_FILENO], STDIN_FILENO) < 0)
                    {
                        std::exit(EXIT_FAILURE);
                    }
                    ::close(fildes[STDIN_FILENO]);

                    const char* buf = text.c_str();
                    ::size_t len = text.length();
                    ::ssize_t s;
                    do
                    {
                        s = ::write(fildes[STDOUT_FILENO], buf, len);
                        if (s < 0)
                        {
                            std::exit(EXIT_FAILURE);
                        }
                        buf += s;
                        len -= s;
                    } while (len != 0);

                    ::close(fildes[STDOUT_FILENO]);
                    int exit_status = ::system(this->path.c_str());
                    std::exit(exit_status);
                }
                else
                {
                    // parent
                    ::close(fildes[STDOUT_FILENO]);
                    std::vector<std::string> responses;

                    char buf[250];
                    ::ssize_t s;
                    do
                    {
                        s = ::read(fildes[STDIN_FILENO], buf, sizeof(buf));
                        if (s < 0)
                        {
                            break;
                        }
                        responses.push_back(std::string(beginof(buf), s));
                    } while (s != 0);

                    foreach (std::vector<std::string>::const_iterator, it, responses)
                    {
                        const std::string& response = *it;
                        if (!response.empty())
                        {
                            this->send_packet(layer, ft::irc::message("PRIVMSG") << target << response);
                        }
                    }
                }
            }

            void on_invite(ft::serv::event_layer& layer, ft::irc::message& message)
            {
                const std::string sender = ft::irc::string_utils::pick_nick(message.get_prefix());
                const std::string& channel = message[1];

                this->set_owner(channel, sender);

                // Accept invite
                this->send_packet(layer, ft::irc::message("JOIN") << channel);
            }

            void on_kick(ft::serv::event_layer& layer, ft::irc::message& message)
            {
                const std::string& channel = message[0];
                const std::string& target = message[1];

                const bool me = ft::irc::string_utils::is_same(target, this->get_nick());
                if (me || this->is_owner(channel, target))
                {
                    this->leave_channel(channel);

                    if (!me)
                    {
                        // Cascade
                        this->send_packet(layer, ft::irc::message("PART") << channel);
                    }
                }
            }

            void on_part(ft::serv::event_layer& layer, ft::irc::message& message)
            {
                const std::string sender = ft::irc::string_utils::pick_nick(message.get_prefix());
                const std::string& channel = message[0];

                if (this->is_owner(channel, sender))
                {
                    this->leave_channel(channel);

                    // Cascade
                    this->send_packet(layer, ft::irc::message("PART") << channel);
                }
            }

            void on_quit(ft::serv::event_layer& layer, ft::irc::message& message)
            {
                const std::string sender = ft::irc::string_utils::pick_nick(message.get_prefix());

                std::vector<std::string> channels = this->find_own_channels(sender);
                std::ostringstream oss;
                bool first = true;
                foreach (std::vector<std::string>::const_iterator, it, channels)
                {
                    this->leave_channel(*it);
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

                // Cascade
                this->send_packet(layer, ft::irc::message("PART") << oss.str());
            }

            void on_nick(ft::serv::event_layer& layer, ft::irc::message& message)
            {
                static_cast<void>(layer);

                const std::string old_nick = ft::irc::string_utils::pick_nick(message.get_prefix());
                const std::string& new_nick = message[0];

                this->update_nick(old_nick, new_nick);
            }

            void on_name_reply(ft::serv::event_layer& layer, ft::irc::message& message)
            {
                static_cast<void>(layer);

                const std::string& channel = message[2];
                ft::irc::message::param_vector members = ft::irc::message::split(message[3], ' ');

                std::vector<std::string>& list = this->names_map[channel];
                foreach (ft::irc::message::param_vector::const_iterator, it, members)
                {
                    std::string nick = *it;
                    if (!nick.empty() && (nick[0] == '@' || nick[0] == '+'))
                    {
                        nick.erase(0, 1);
                    }
                    list.push_back(nick);
                }
            }

            void on_end_of_names(ft::serv::event_layer& layer, ft::irc::message& message)
            {
                const std::string& channel = message[1];

                std::vector<std::string> list = this->names_map[channel]; // copy
                this->names_map.erase(channel);

                std::vector<std::string>::const_iterator it = std::find(list.begin(), list.end(), this->get_owner(channel));
                if (it == list.end())
                {
                    // owner not found, Cascade
                    this->send_packet(layer, ft::irc::message("PART") << channel);
                }
            }

            void on_bad_error(ft::serv::event_layer& layer, ft::irc::message& message)
            {
                const std::string& channel = message[1];
                const std::string& reason = message[2];

                const std::string& owner = this->get_owner(channel);

                if (!owner.empty())
                {
                    // help to owner
                    std::ostringstream oss;
                    oss << "HELP!! I can't do anything on this channel: " << channel << ". Because: " << reason;
                    this->send_packet(layer, ft::irc::message("PRIVMSG") << owner << oss.str());
                }
            }

            void on_nick_error(ft::serv::event_layer& layer, ft::irc::message& message)
            {
                static_cast<void>(message);

                layer.post_disconnect();
            }

            void on_packet(ft::serv::event_layer& layer, ft::irc::message& message)
            {
                const std::string command = message.get_command();

                void (ft::irc::bot::*proc)(ft::serv::event_layer&, ft::irc::message&) = null;
                if (command == "PRIVMSG")
                {
                    proc = &ft::irc::bot::on_privmsg;
                }
                else if (command == "INVITE")
                {
                    proc = &ft::irc::bot::on_invite;
                }
                else if (command == "KICK")
                {
                    proc = &ft::irc::bot::on_kick;
                }
                else if (command == "PART")
                {
                    proc = &ft::irc::bot::on_part;
                }
                else if (command == "QUIT")
                {
                    proc = &ft::irc::bot::on_quit;
                }
                else if (command == "NICK")
                {
                    proc = &ft::irc::bot::on_nick;
                }
                else if (std::isdigit(command[0]))
                {
                    int rpl = std::atoi(command.c_str());
                    switch (rpl)
                    {
                    case RPL_NAMREPLY:
                        proc = &ft::irc::bot::on_name_reply;
                        break;
                    case RPL_ENDOFNAMES:
                        proc = &ft::irc::bot::on_end_of_names;
                        break;
                    case ERR_NOSUCHCHANNEL:
                    case ERR_TOOMANYCHANNELS:
                    case ERR_CHANNELISFULL:
                    case ERR_INVITEONLYCHAN:
                    case ERR_BANNEDFROMCHAN:
                    case ERR_BADCHANNELKEY:
                    case ERR_CANNOTSENDTOCHAN:
                        proc = &ft::irc::bot::on_bad_error;
                        break;
                    case ERR_ERRONEUSNICKNAME:
                    case ERR_NICKNAMEINUSE:
                        proc = &ft::irc::bot::on_nick_error;
                        break;
                    }
                }

                if (proc != null)
                {
                    (this->*proc)(layer, message);
                }
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

ft::shared_ptr<ft::serv::event_worker_group> child_group;

static void _on_signal(int signo)
{
    static_cast<void>(signo);
    child_group->shutdown_all();
}

int main(int argc, char* argv[])
{
    if (argc != 8)
    {
        ft::serv::logger::warn("Usage: %s <host> <port> <pass> <nick> <username> <realname> <path>", argv[0]);
        return EXIT_FAILURE;
    }

    child_group = ft::make_shared<ft::serv::event_worker_group>();
    child_group->put_worker(ft::make_shared<ft::serv::event_worker>());

    ft::irc::bot bot(argv[3], argv[4], argv[5], argv[6], argv[7]);
    ft::serv::bootstrap boot(child_group, child_group, null, &ft::irc::_make_client);
    std::signal(SIGINT, &_on_signal);
    std::signal(SIGCHLD, SIG_IGN); // reaping children
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
