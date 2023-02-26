/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "processor.hpp"

#include "processor_channel.hpp"
#include "processor_misc.hpp"
#include "processor_register.hpp"
#include "processor_send_message.hpp"
#include "processor_user.hpp"

#include <cctype>
#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>
#include <thread/once.hpp>

#include <cstdlib>
#include <string>
#include <utility>

ft::irc::processor_base::~processor_base()
{
}

std::size_t ft::irc::processor_base::get_min_params() const throw()
{
    return 0;
}

std::size_t ft::irc::processor_base::get_max_params() const throw()
{
    return FT_IRC_MESSAGE_PARAM_LIMIT;
}

bool ft::irc::processor_base::is_registered_only() const throw()
{
    // NOTE: false when command is { NICK, QUIT, USER, PASS }
    return true;
}

bool ft::irc::processor_base::is_operator_only() const throw()
{
    // NOTE: true when command is { KILL, SQUIT, CONNECT, REHASH, RESTART }
    return false;
}

ft::shared_ptr<ft::irc::processor_base> ft::irc::processor_dictionary::null_processor_ptr;
ft::irc::processor_dictionary::dictionary_type ft::irc::processor_dictionary::dictionary;
ft::once_flag ft::irc::processor_dictionary::init_once_flag;

static std::string _make_key(const char* s)
{
    std::string str = s;
    foreach (std::string::iterator, it, str)
    {
        *it = std::toupper(*it);
    }
    return str;
}

void ft::irc::processor_dictionary::init()
{
#define FT_REGISTER_PROCESSOR(_cmd) ft::irc::processor_dictionary::dictionary.insert(std::make_pair(_make_key(#_cmd), ft::make_shared<ft::irc::processor_##_cmd>()))
    // register
    FT_REGISTER_PROCESSOR(pass);
    FT_REGISTER_PROCESSOR(nick);
    FT_REGISTER_PROCESSOR(user);
    FT_REGISTER_PROCESSOR(oper);
    FT_REGISTER_PROCESSOR(quit);

    // channel
    FT_REGISTER_PROCESSOR(join);
    FT_REGISTER_PROCESSOR(part);
    FT_REGISTER_PROCESSOR(mode);
    FT_REGISTER_PROCESSOR(topic);
    FT_REGISTER_PROCESSOR(names);
    FT_REGISTER_PROCESSOR(invite);
    FT_REGISTER_PROCESSOR(list);
    FT_REGISTER_PROCESSOR(kick);

    // send_message
    FT_REGISTER_PROCESSOR(privmsg);
    FT_REGISTER_PROCESSOR(notice);

    // user
    FT_REGISTER_PROCESSOR(who);
    FT_REGISTER_PROCESSOR(whois);
    FT_REGISTER_PROCESSOR(whowas);

    // misc
    FT_REGISTER_PROCESSOR(kill);
    FT_REGISTER_PROCESSOR(ping);
    FT_REGISTER_PROCESSOR(pong);
    FT_REGISTER_PROCESSOR(error);
#undef FT_REGISTER_PROCESSOR
}

const ft::shared_ptr<ft::irc::processor_base>& ft::irc::processor_dictionary::get(const std::string& command)
{
    ft::call_once(ft::irc::processor_dictionary::init_once_flag, &ft::irc::processor_dictionary::init);

    dictionary_type::iterator it = ft::irc::processor_dictionary::dictionary.find(command);
    if (it != ft::irc::processor_dictionary::dictionary.end())
    {
        return it->second;
    }
    return ft::irc::processor_dictionary::null_processor_ptr;
}

void ft::irc::processor_dictionary::execute(ft::irc::user& user, const ft::irc::message& message)
{
    const std::string& command = message.get_command();
    const ft::shared_ptr<ft::irc::processor_base>& processor = ft::irc::processor_dictionary::get(command);

    if (!processor)
    {
        user.send_message(ft::irc::make_error::unknown_command(command));
        return;
    }

    if (message.size_param() < processor->get_min_params())
    {
        user.send_message(ft::irc::make_error::need_more_parameters(command));
        return;
    }

    // TODO: check operator, register

    processor->execute(user, message);
}
