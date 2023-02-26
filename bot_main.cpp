/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "irc_constants.hpp"

#include "bot_handler.hpp"

#include "bot.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>

#if not FT_IRC_HARDWARE_CONCURRENCY > 0
#include <unistd.h> // sysconf
#endif

namespace ft
{
    namespace irc
    {
        static ft::shared_ptr<ft::serv::event_channel_base> _make_client(ft::serv::ident_t ident, const std::string& host, int serv, void* arg)
        {
            ft::shared_ptr<ft::serv::event_channel_base> server = ft::make_shared<ft::serv::stream_channel>(ident, host, serv);
            server->add_last_handler(ft::make_shared<ft::irc::bot_handler>(*static_cast<ft::irc::bot*>(arg)));
            return server;
        }
    }
}

int main()
{
    // FIXME: 임시 코드
    ft::shared_ptr<ft::serv::event_worker_group> boss_group = ft::make_shared<ft::serv::event_worker_group>();
    ft::shared_ptr<ft::serv::event_worker_group> child_group = ft::make_shared<ft::serv::event_worker_group>();
    // TODO: sigaction
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
    ft::serv::bootstrap boot(boss_group, child_group, null, &ft::irc::_make_client);
    ft::irc::bot bot;
    boot.start_client("localhost", "4242", &bot);
    return 0;
}
