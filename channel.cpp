/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "channel.hpp"

ft::irc::channel::channel(const std::string& host, const std::string& serv, const std::string& pass)
    : host(host),
      serv(serv),
      pass(pass)
{
}

ft::irc::channel::~channel()
{
}

const std::string& ft::irc::channel::get_host() const throw()
{
    return this->host;
}

const std::string& ft::irc::channel::get_serv() const throw()
{
    return this->serv;
}

const std::string& ft::irc::channel::get_pass() const throw()
{
    return this->pass;
}

const std::string& ft::irc::channel::get_channelname() const throw()
{
    return this->channelname;
}

const channel_mode& ft::irc::channel::get_mode() const throw()
{
    return this->mode;
}

const client_dictionary& ft::irc::channel::get_clients() const throw()
{
    return this->clients;
}
