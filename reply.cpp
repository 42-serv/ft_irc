/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "reply.hpp"

#include "irc_constants.hpp"

#include "message.hpp"

#include <thread/thread_specific.hpp>

#include <string>

ft::thread_specific_ptr<std::string> ft::irc::make_reply_base::my_server_name;
ft::thread_specific_ptr<std::string> ft::irc::make_reply_base::my_user_nick;

void ft::irc::make_reply_base::set_server_name(const std::string& str)
{
    if (ft::irc::make_reply_base::my_server_name.get())
    {
        *ft::irc::make_reply_base::my_server_name = str;
    }
    else
    {
        ft::irc::make_reply_base::my_server_name.reset(new std::string(str));
    }
}

void ft::irc::make_reply_base::set_user_nick(const std::string& str)
{
    if (ft::irc::make_reply_base::my_user_nick.get())
    {
        *ft::irc::make_reply_base::my_user_nick = str;
    }
    else
    {
        ft::irc::make_reply_base::my_user_nick.reset(new std::string(str));
    }
}
