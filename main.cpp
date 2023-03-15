/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "irc_constants.hpp"

#include "server.hpp"
#include "server_handler.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>

#include <csignal>
#include <cstdlib>

#if not FT_IRC_HARDWARE_CONCURRENCY > 0
#include <unistd.h> // sysconf
#endif

namespace ft
{
    namespace irc
    {
        static ft::shared_ptr<ft::serv::event_channel_base> _make_server(ft::serv::ident_t ident, const std::string& host, int serv, const ft::shared_ptr<ft::serv::event_worker_group>& child_group, void* arg)
        {
            const ft::shared_ptr<ft::serv::event_channel_base> server = ft::make_shared<ft::serv::server_channel>(ident, host, serv, child_group);
            server->add_last_handler(ft::make_shared<ft::irc::server_handler>(*static_cast<ft::irc::server*>(arg)));
            return server;
        }
    }
}

ft::shared_ptr<ft::serv::event_worker_group> boss_group;
ft::shared_ptr<ft::serv::event_worker_group> child_group;

static void _on_signal(int signo)
{
    static_cast<void>(signo);
    boss_group->shutdown_all();
    child_group->shutdown_all();
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        ft::serv::logger::warn("Usage: %s <port> <pass>", argv[0]);
        return EXIT_FAILURE;
    }
    boss_group = ft::make_shared<ft::serv::event_worker_group>();
    child_group = ft::make_shared<ft::serv::event_worker_group>();
    boss_group->put_worker(ft::make_shared<ft::serv::event_worker>());
    const long hardware_concurrency =
#if FT_IRC_HARDWARE_CONCURRENCY > 0
        FT_IRC_HARDWARE_CONCURRENCY
#else
        ::sysconf(_SC_NPROCESSORS_ONLN)
#endif
        ;
    for (long i = 0; i < hardware_concurrency; i++)
    {
        child_group->put_worker(ft::make_shared<ft::serv::event_worker>());
    }
    ft::irc::server server(argv[2]);
    ft::serv::bootstrap boot(boss_group, child_group, &ft::irc::_make_server, null);
    std::signal(SIGINT, &_on_signal);
    try
    {
        bool success = true;

        if (!boot.start_server("", argv[1], &server))
        {
            ft::serv::logger::error("bind failed. Is port \"%s\" wrong?", argv[1]);
            success = false;
        }

        if (success)
        {
            ft::serv::logger::info("Server successfully started.");
            boot.set_success();
        }
    }
    catch (const ft::serv::syscall_failed& e)
    {
        ft::serv::logger::error("bind failed. System error occured: %s", e.what());
    }
    boot.finalize();
    ft::serv::logger::warn("Server finalized.");
    return EXIT_SUCCESS;
}
