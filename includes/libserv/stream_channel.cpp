/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "stream_channel.hpp"

#include "byte_buffer.hpp"
#include "event_channel_base.hpp"
#include "event_layer.hpp"
#include "serv_exception.hpp"
#include "serv_types.hpp"

#include <smart_ptr/make_shared.hpp>
#include <smart_ptr/shared_ptr.hpp>

#include <cstddef>
#include <string>

ft::serv::stream_channel::stream_channel(ident_t ident, const std::string& host, int serv, std::size_t init_buf_capacity)
    : event_channel_base(ident, host, serv),
      init_buf_capacity(init_buf_capacity)
{
}

ft::serv::stream_channel::~stream_channel()
{
}

void ft::serv::stream_channel::begin_read()
{
    const ft::shared_ptr<event_layer>& pipeline = this->get_pipeline();
    bool gracefully_shutdown = false;
    do
    {
        const ft::shared_ptr<byte_buffer> buf = ft::make_shared<byte_buffer>(this->init_buf_capacity);
        const long len = this->do_read(buf->raw_buffer(), buf->raw_length());
        if (len < 0)
        {
            const error_t err = -len;
            if (err == EAGAIN)
            {
                break;
            }
            const ft::shared_ptr<syscall_failed> e = ft::make_shared<syscall_failed>(err);
#ifdef FT_TRACE
            this->trace_log("[RecvErr] ", e->what());
#endif
            pipeline->notify_error(e);
            return;
        }
        if (len == 0)
        {
            gracefully_shutdown = true;
            break;
        }
#ifdef FT_TRACE
        this->trace_dump_bytes("[Recv] ", buf->raw_buffer(), len);
#endif
        buf->raw_shrink(len);
        pipeline->notify_read(buf);
    } while (!0);
    pipeline->notify_read_complete();
    if (gracefully_shutdown)
    {
        this->shutdown_input();
        pipeline->notify_error(ft::make_shared<orderly_shutdown>());
    }
}
