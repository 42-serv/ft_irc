/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "file_channel.hpp"

#include "serv_exception.hpp"
#include "serv_types.hpp"

#include <sys/types.h>
#include <unistd.h>

#include <cstddef>
#include <string>

ft::serv::file_channel::file_channel(ident_t ident, const std::string& host, int serv, std::size_t init_buf_capacity)
    : stream_channel(ident, host, serv, init_buf_capacity)
{
}

ft::serv::file_channel::~file_channel()
{
}

long ft::serv::file_channel::do_read(void* const buf, const std::size_t len) throw()
{
    staticassert(sizeof(long) >= sizeof(::ssize_t));

    const ident_t ident = this->get_ident();
    ::ssize_t value;
    do
    {
        value = ::read(ident, buf, len);
        if (value < 0)
        {
            const syscall_failed e;
            switch (e.error())
            {
            case EINTR:
                continue;

            default:
                return -e.error();
            }
        }
        break;
    } while (!0);

    return value;
}

long ft::serv::file_channel::do_write(const void* const buf, const std::size_t len) throw()
{
    staticassert(sizeof(long) >= sizeof(::ssize_t));

    const ident_t ident = this->get_ident();
    ::ssize_t value;
    do
    {
        value = ::write(ident, buf, len);
        if (value < 0)
        {
            const syscall_failed e;
            switch (e.error())
            {
            case EINTR:
                continue;

            default:
                return -e.error();
            }
        }
        break;
    } while (!0);

    return value;
}
