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
            ft::shared_ptr<ft::serv::event_channel_base> server = ft::make_shared<ft::serv::server_channel>(ident, host, serv, child_group);
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
    std::cout << "signaled: " << signo << std::endl;
    boss_group->shutdown_all();
    child_group->shutdown_all();
}

int main(int argc, char* argv[])
{
    // FIXME: 임시 코드
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <port> <pass>" << std::endl;
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
    std::signal(SIGINT, &_on_signal); // TODO: sigaction
    try
    {
        bool success = true;

        if (!boot.start_server("", argv[1], &server))
        {
            std::cerr << "bind failed. Is port \"" << argv[1] << "\" wrong?" << std::endl;
            success = false;
        }

        if (success)
        {
            std::cout << "Server successfully started." << std::endl;
            boot.set_success();
        }
    }
    catch (const ft::serv::syscall_failed& e)
    {
        std::cerr << "bind failed. System error occured: " << e.what() << std::endl;
    }
    boot.finalize();
    std::cerr << "FINAL" << std::endl;
    boss_group.reset();
    child_group.reset();
    return EXIT_SUCCESS;
}
