/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "bot.hpp"
#include "libserv/serv_types.hpp"

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

void ft::irc::bot::add_inviter(const std::string& channel, const std::string& inviter) throw()
{
    this->inviters.insert(std::make_pair(channel, inviter));
}

void ft::irc::bot::remove_inviter(const std::string& channel)
{
    this->inviters.erase(channel);
}

bool ft::irc::bot::check_is_inviter(const std::string& channel, const std::string& inviter)
{
    ft::irc::bot::inviter_dictionary::iterator it = this->inviters.find(channel);

    return it != this->inviters.end() && it->second == inviter;
}

const std::string ft::irc::bot::find_channels(const std::string& sender) throw()
{
    std::vector<std::string> channels_to_remove;
    std::string result;

    foreach (ft::irc::bot::inviter_dictionary::const_iterator, it, this->inviters)
    {
        if (it->second == sender)
        {
            channels_to_remove.push_back(it->first);
        }
    }
    foreach (std::vector<std::string>::iterator, it, channels_to_remove)
    {
        this->inviters.erase(*it);
        if (!result.empty())
        {
            result += ",";
        }
        result += *it;
    }
    return result;
}
