/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "irc_constants.hpp"

#include "bot_handler.hpp"

#include "bot.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>

namespace ft
{
    namespace irc
    {
        static ft::shared_ptr<ft::serv::event_channel_base> _make_client(ft::serv::ident_t ident, const std::string& host, int serv, void* arg)
        {
            ft::shared_ptr<ft::serv::event_channel_base> child = ft::make_shared<ft::serv::stream_channel>(ident, host, serv);
            child->add_last_handler(ft::make_shared<ft::irc::string_line_encoder>());
            child->add_last_handler(ft::make_shared<ft::irc::string_line_decoder>());
            child->add_last_handler(ft::make_shared<ft::irc::message_encoder>());
            child->add_last_handler(ft::make_shared<ft::irc::message_decoder>());
            child->add_last_handler(ft::make_shared<ft::irc::bot_handler>(*static_cast<ft::irc::bot*>(arg)));
            return child;
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        ft::serv::logger::warn("Usage: %s <host> <port> <pass>", argv[0]);
        return EXIT_FAILURE;
    }
    ft::shared_ptr<ft::serv::event_worker_group> child_group = ft::make_shared<ft::serv::event_worker_group>();
    child_group->put_worker(ft::make_shared<ft::serv::event_worker>());
    ft::irc::bot bot(argv[3]);
    {
        ft::serv::bootstrap boot(child_group, child_group, null, &ft::irc::_make_client);
        try
        {
            if (!boot.start_client(argv[1], argv[2], &bot))
            {
                ft::serv::logger::error("bind failed. Is host \"%s\" or port \"%s\" wrong?", argv[1], argv[2]);
                return 1;
            }
            boot.set_success();
        }
        catch (const ft::serv::syscall_failed& e)
        {
            ft::serv::logger::error("bind failed. System error occured: %s", e.what());
            return 1;
        }
    }
    return 0;
}
