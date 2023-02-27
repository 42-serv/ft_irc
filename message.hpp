/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include <libserv/libserv.hpp>

#include <string>

namespace ft
{
    namespace irc
    {
        class message
        {
        public:
            typedef ft::serv::dynamic_array<std::string>::type param_vector;

        public:
            static param_vector split(const std::string& str, std::string::value_type delim = ' ', std::string::value_type limit = '\0');
            static bool try_parse(const std::string& str, message& out_msg);

        private:
            std::string command;
            std::string prefix;
            param_vector params;
            bool end;

        public:
            message();
            message(int command);
            message(const std::string& command);
            message(const std::string& prefix, const std::string& command, const param_vector& params);

            std::string& operator[](param_vector::size_type n);
            const std::string& operator[](param_vector::size_type n) const;

            message& operator>>(const std::string& prefix);

            message& operator<<(const std::string& str);
            template <typename T>
            message& operator<<(const T& obj);

            const std::string& get_command() const;

            const std::string& get_prefix() const;
            void set_prefix(const std::string& prefix);

            std::size_t param_size() const;
            void add_param(const std::string& param);
            void remove_all_param();

            std::string to_string() const;
            std::string to_pretty_string() const;
        };
    }
}

#include "message.tpp"
