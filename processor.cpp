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
#define _REGISTER_PROCESSOR(_cmd) ft::irc::processor_dictionary::dictionary.insert(std::make_pair(_make_key(#_cmd), ft::make_shared<ft::irc::processor_##_cmd>()))
    // register
    _REGISTER_PROCESSOR(pass);
    _REGISTER_PROCESSOR(nick);
    _REGISTER_PROCESSOR(user);
    _REGISTER_PROCESSOR(oper);
    _REGISTER_PROCESSOR(quit);

    // channel
    _REGISTER_PROCESSOR(join);
    _REGISTER_PROCESSOR(part);
    _REGISTER_PROCESSOR(mode);
    _REGISTER_PROCESSOR(topic);
    _REGISTER_PROCESSOR(names);
    _REGISTER_PROCESSOR(invite);
    _REGISTER_PROCESSOR(list);
    _REGISTER_PROCESSOR(kick);

    // send_message
    _REGISTER_PROCESSOR(privmsg);
    _REGISTER_PROCESSOR(notice);

    // user
    _REGISTER_PROCESSOR(who);
    _REGISTER_PROCESSOR(whois);
    _REGISTER_PROCESSOR(whowas);

    // misc
    _REGISTER_PROCESSOR(kill);
    _REGISTER_PROCESSOR(ping);
    _REGISTER_PROCESSOR(pong);
    _REGISTER_PROCESSOR(error);
#undef _REGISTER_PROCESSOR
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
