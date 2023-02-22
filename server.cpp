/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "server_handler.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>

namespace ft
{
    namespace irc
    {
        static ft::shared_ptr<ft::serv::event_channel_base> _make_server(ft::serv::ident_t ident, const std::string& host, int serv, const ft::shared_ptr<ft::serv::event_worker_group>& child_group)
        {
            ft::shared_ptr<ft::serv::event_channel_base> server = ft::make_shared<ft::serv::server_channel>(ident, host, serv, child_group);
            server->add_last_handler(ft::make_shared<server_handler>());
            return server;
        }
    }
}

int main()
{
    // FIXME: 임시 코드
    ft::shared_ptr<ft::serv::event_worker_group> boss_group = ft::make_shared<ft::serv::event_worker_group>();
    ft::shared_ptr<ft::serv::event_worker_group> child_group = ft::make_shared<ft::serv::event_worker_group>();
    boss_group->put_worker(ft::make_shared<ft::serv::event_worker>());
    for (int i = 0; i < 5; i++)
    {
        child_group->put_worker(ft::make_shared<ft::serv::event_worker>());
    }
    ft::serv::bootstrap boot(boss_group, child_group, &ft::irc::_make_server, null);
    boot.start_server("localhost", "4242");
    return 0;
}
