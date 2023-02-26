/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>
#include <thread/readwrite_lock.hpp>

#include <string>

namespace ft
{
    namespace irc
    {
        class bot : public ft::enable_shared_from_this<bot>
        {
        public:
        private:
        public:
            bot() {}
            ~bot() {}

        public:
        private:
            bot(const bot&);
            bot& operator=(const bot&);
        };
    }
}
