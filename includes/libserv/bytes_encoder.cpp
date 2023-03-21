/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "bytes_encoder.hpp"

#include "byte_buffer.hpp"
#include "event_handler_base.hpp"
#include "event_layer.hpp"
#include "serv_types.hpp"

#include <smart_ptr/make_shared.hpp>
#include <smart_ptr/shared_ptr.hpp>

ft::serv::bytes_encoder::bytes_encoder()
{
}

ft::serv::bytes_encoder::~bytes_encoder()
{
}

void ft::serv::bytes_encoder::on_write(event_layer& layer, ft::shared_ptr<const void> arg)
{
    const ft::shared_ptr<byte_buffer> buf = ft::make_shared<byte_buffer>();
    this->encode(arg, *buf);
    layer.post_write(buf);
}

void ft::serv::bytes_encoder::encode(const ft::shared_ptr<const void>&, byte_buffer&)
{
    throw std::runtime_error("not implemented encode()");
}
