/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "bot.hpp"

#include <string>

ft::irc::bot::bot(const std::string& pass)
    : pass(pass),
      nick("ircBot"),
      username("user"),
      hostname("local"),
      servername("user"),
      realname("42bot")
{
}

ft::irc::bot::~bot()
{
}

const std::string& ft::irc::bot::get_pass() const throw()
{
    return this->pass;
}

const std::string& ft::irc::bot::get_nick() const throw()
{
    return this->nick;
}

const std::string& ft::irc::bot::get_username() const throw()
{
    return this->username;
}

const std::string& ft::irc::bot::get_hostname() const throw()
{
    return this->hostname;
}

const std::string& ft::irc::bot::get_servername() const throw()
{
    return this->servername;
}

const std::string& ft::irc::bot::get_realname() const throw()
{
    return this->realname;
}
