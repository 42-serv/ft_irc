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

#include "debug.hpp"

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
            child->add_first_handler(ft::make_shared<byte_buffer_dump_logger_handler>()); // NOTE: DEBUG
            child->add_last_handler(ft::make_shared<ft::irc::bot_handler>(*static_cast<ft::irc::bot*>(arg)));
            return child;
        }
    }
}

int main(int argc, char* argv[])
{
    // FIXME: 임시 코드
    if (argc != 4)
    {
        std::cout << "Usage: " << argv[0] << " <host> <port> <pass>" << std::endl;
        return EXIT_FAILURE;
    }
    ft::shared_ptr<ft::serv::event_worker_group> child_group = ft::make_shared<ft::serv::event_worker_group>();
    // TODO: sigaction
    child_group->put_worker(ft::make_shared<ft::serv::event_worker>());
    ft::irc::bot bot(argv[3]);
    {
        ft::serv::bootstrap boot(child_group, child_group, null, &ft::irc::_make_client);
        try
        {
            if (!boot.start_client(argv[1], argv[2], &bot))
            {
                std::cerr << "bind failed. Is host \"" << argv[1] << "\" or port \"" << argv[2] << "\" wrong?" << std::endl;
                child_group->shutdown_all();
            }
        }
        catch (const ft::serv::syscall_failed& e)
        {
            std::cerr << "bind failed. System error occured: " << e.what() << std::endl;
            child_group->shutdown_all();
        }
    }
    return 0;
}
