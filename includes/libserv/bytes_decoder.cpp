/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "bytes_decoder.hpp"

#include "byte_buffer.hpp"
#include "event_layer.hpp"
#include "serv_types.hpp"

#include <smart_ptr/shared_ptr.hpp>

#include <cassert>
#include <stdexcept>

ft::serv::bytes_decoder::bytes_decoder() : cumulative_buf(), output()
{
}

ft::serv::bytes_decoder::~bytes_decoder()
{
}

void ft::serv::bytes_decoder::on_read(event_layer& layer, ft::shared_ptr<void> arg)
{
    const ft::shared_ptr<byte_buffer> buf = ft::static_pointer_cast<byte_buffer>(arg);
    if (this->cumulative_buf.empty())
    {
        this->cumulative_buf = *buf;
    }
    else
    {
        this->cumulative_buf.append_from(*buf);
    }

    do
    {
        this->output.clear();
        const byte_buffer::size_type size_prev = this->cumulative_buf.size();
        this->decode(this->cumulative_buf, this->output);
        if (this->cumulative_buf.size() == size_prev)
        {
            assert(this->output.empty());

            break;
        }

        assert(this->cumulative_buf.size() < size_prev);

        foreach (output_buffer::iterator, it, this->output)
        {
            layer.notify_read(*it);
        }
    } while (!0);
    this->output.clear();
}

void ft::serv::bytes_decoder::on_read_complete(event_layer& layer)
{
    this->cumulative_buf.discard();
    layer.notify_read_complete();
}

void ft::serv::bytes_decoder::decode(byte_buffer&, output_buffer&)
{
    throw std::runtime_error("not implemented decode()");
}
