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

        class bootstrap
        {
        public:
            typedef ft::shared_ptr<event_channel_base> (*make_server_t)(ident_t ident, const std::string& host, int serv, const ft::shared_ptr<event_worker_group>& child_group, void* arg);
            typedef ft::shared_ptr<event_channel_base> (*make_client_t)(ident_t ident, const std::string& host, int serv, void* arg);

        private:
            ft::shared_ptr<event_worker_group> boss_group;
            ft::shared_ptr<event_worker_group> child_group;
            make_server_t make_server;
            make_client_t make_client;
            bool success;
            bool ended;

        public:
            bootstrap(const ft::shared_ptr<event_worker_group>& boss_group, const ft::shared_ptr<event_worker_group>& child_group, make_server_t make_server, make_client_t make_client);
            ~bootstrap();

        public:
            bool start_server(const std::string& host_str, const std::string& serv_str, void* arg);
            bool start_client(const std::string& host_str, const std::string& serv_str, void* arg);
            void set_success();
            void finalize();

        private:
            bootstrap(const bootstrap&);
            bootstrap& operator=(const bootstrap&);
        };
    }
}
