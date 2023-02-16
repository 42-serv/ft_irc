/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "message.hpp"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

std::vector<std::string> ft::irc::message::split(const std::string& str, std::string::value_type delim = ' ', std::string::value_type limit = '\0')
{
    std::vector<std::string> vec;

    for (std::string::size_type beg = str.find_first_not_of(delim), end; beg != std::string::npos; beg = str.find_first_not_of(delim, end))
    {
        if (limit != '\0' && str[beg] == limit)
        {
            beg++;
            end = std::string::npos;
        }
        else
        {
            end = str.find_first_of(delim, beg);
        }

        if (beg != end)
        {
            vec.push_back(str.substr(beg, end - beg));
        }
    }

    return vec;
}

static bool _validate_command(std::string& command)
{
    if (std::isalpha(command[0]))
    {
        for (std::string::iterator it = command.begin(); it != command.end(); ++it)
        {
            if (!std::isalpha(*it))
            {
                return false;
            }
            *it = std::toupper(*it);
        }
    }
    else if (std::isdigit(command[0]))
    {
        if (command.size() != 3)
        {
            return false;
        }
        for (std::string::iterator it = command.begin(); it != command.end(); ++it)
        {
            if (!std::isdigit(*it))
            {
                return false;
            }
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool ft::irc::message::try_parse(const std::string& line, message& out_msg)
{
    if (line.empty())
    {
        return false;
    }

    std::string::size_type pos_begin = 0;

    std::string prefix;
    if (line[0] == ':')
    {
        std::string::size_type pos_end_prefix = line.find_first_of(' ', pos_begin);
        if (pos_end_prefix == std::string::npos)
        {
            return false;
        }
        prefix = line.substr(pos_begin, pos_end_prefix);

        pos_begin = line.find_first_not_of(' ', pos_end_prefix + 1);
        if (pos_begin == std::string::npos)
        {
            return false;
        }
    }

    std::string command;
    {
        std::string::size_type pos_end_command = line.find_first_of(' ', pos_begin);
        command = line.substr(pos_begin, pos_end_command);

        if (!_validate_command(command))
        {
            return false;
        }

        pos_begin = pos_end_command;
    }

    std::vector<std::string> params = message::split(line.substr(pos_begin), ' ', ':');
    if (params.size() > 15)
    {
        // TODO: configurable limit
        return false;
    }

    out_msg = message(prefix, command, params);
    return true;
}

ft::irc::message::message(int command, const std::string& prefix)
    : command(),
      prefix(prefix),
      params(),
      end(false)
{
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(3) << std::dec << command; // %03d
    this->command = oss.str();

    assert(_validate_command(this->command));
}

ft::irc::message::message(const std::string& prefix, const std::string& command, const std::vector<std::string>& params)
    : command(command),
      prefix(prefix),
      params(params),
      end(true) // for recv
{
    assert(_validate_command(this->command));
}

std::string& ft::irc::message::operator[](param_vector::size_type n)
{
    const message* const_this = this;
    return const_cast<std::string&>(const_this->operator[](n));
}

const std::string& ft::irc::message::operator[](param_vector::size_type n) const
{
    return this->params[n];
}

ft::irc::message& ft::irc::message::operator<<(const std::string& str)
{
    this->add_param(str);
    return *this;
}

const std::string& ft::irc::message::get_command() const
{
    return this->command;
}

const std::string& ft::irc::message::get_prefix() const
{
    return this->prefix;
}

void ft::irc::message::set_prefix(const std::string& prefix)
{
    if (prefix.find(' ') != std::string::npos)
    {
        throw std::runtime_error("prefix with space");
    }

    this->prefix = prefix;
}

std::size_t ft::irc::message::size_param() const
{
    return this->params.size();
}

void ft::irc::message::add_param(const std::string& param)
{
    if (this->end)
    {
        throw std::runtime_error("ended message builder");
    }

    this->params.push_back(param);
    this->end = param.find(' ') != std::string::npos;
}

void ft::irc::message::remove_all_param()
{
    this->params.clear();
    this->end = false;
}

std::string ft::irc::message::to_string() const
{
    std::ostringstream oss;
    if (!this->prefix.empty())
    {
        oss << ":" << this->prefix << " ";
    }
    oss << this->command;
    for (std::vector<std::string>::const_iterator it = this->params.begin(); it != this->params.end(); ++it)
    {
        oss << ((this->end && it + 1 == this->params.end()) ? " :" : " ") << *it;
    }
    return oss.str();
}

std::string ft::irc::message::to_pretty_string() const
{
    std::ostringstream oss;
    oss << "{" << std::endl;
    oss << "\tCOMMAND=\"" << this->get_command() << "\"" << std::endl;
    oss << "\tPREFIX=\"" << this->get_prefix() << "\"" << std::endl;
    oss << "\tPARAMS = [" << this->size_param() << "] {" << std::endl;
    for (std::size_t i = 1; i <= this->size_param(); i++)
    {
        oss << "\t\t[" << i << "] = \"" << this->operator[](i) << "\"" << std::endl;
    }
    oss << "\t}" << std::endl;
    oss << "}" << std::endl;
    return oss.str();
}
