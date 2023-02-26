/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include "libserv/serv_types.hpp"
#include "message.hpp"
#include "user.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>
#include <thread/once.hpp>

#include <cstdlib>
#include <string>

namespace ft
{
    namespace irc
    {
        class processor_base
        {
        public:
            virtual ~processor_base();
            virtual std::size_t get_min_params() const throw();
            virtual std::size_t get_max_params() const throw();
            virtual bool is_registered_only() const throw();
            virtual bool is_operator_only() const throw();
            virtual void execute(ft::irc::user& user, const ft::irc::message& message) const = 0;
        };

        class processor_dictionary
        {
        private:
            typedef ft::serv::fast_dictionary<std::string, ft::shared_ptr<ft::irc::processor_base> >::type dictionary_type;

        private:
            static ft::shared_ptr<ft::irc::processor_base> null_processor_ptr;
            static dictionary_type dictionary;
            static ft::once_flag init_once_flag;

        private:
            static void init();
            static const ft::shared_ptr<ft::irc::processor_base>& get(const std::string& command);

        public:
            static void execute(ft::irc::user& user, const ft::irc::message& message);
        };
    }
}
