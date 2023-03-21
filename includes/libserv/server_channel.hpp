/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "event_channel_base.hpp"
#include "serv_types.hpp"

#include <smart_ptr/shared_ptr.hpp>

#include <string>

namespace ft
{
    namespace serv
    {
        class event_worker_group;

        class server_channel : public event_channel_base
        {
        private:
            ft::shared_ptr<event_worker_group> group;

        public:
            server_channel(ident_t ident, const std::string& host, int serv, const ft::shared_ptr<event_worker_group>& group);
            virtual ~server_channel();

        protected:
            virtual void begin_read();
            virtual ft::shared_ptr<event_channel_base> make_child(ident_t child_ident, const std::string& child_host, int child_serv);

        private:
            server_channel(const server_channel&);
            server_channel& operator=(const server_channel&);
        };
    }
}
