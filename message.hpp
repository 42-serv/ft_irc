/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <string>
#include <vector>

namespace ft
{
    namespace irc
    {
        class message
        {
        public:
            static std::vector<std::string> message::split(const std::string& str, std::string::value_type delim = ' ', std::string::value_type limit = '\0');
            static bool try_parse(const std::string& str, message& out_msg);

        public:
            typedef std::vector<std::string> param_vector;

        private:
            std::string command;
            std::string prefix;
            param_vector params;
            bool end;

        public:
            message(int command, const std::string& prefix = std::string());
            message(const std::string& prefix, const std::string& command, const std::vector<std::string>& params);

            std::string& operator[](param_vector::size_type n);
            const std::string& operator[](param_vector::size_type n) const;

            message& operator<<(const std::string& str);

            const std::string& get_command() const;

            const std::string& get_prefix() const;
            void set_prefix(const std::string& prefix);

            std::size_t size_param() const;
            void add_param(const std::string& param);
            void remove_all_param();

            std::string to_string() const;
            std::string to_pretty_string() const;
        };
    }
}
