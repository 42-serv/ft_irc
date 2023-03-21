/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "event_handler_base.hpp"

#include "event_layer.hpp"
#include "serv_types.hpp"

#include <smart_ptr/shared_ptr.hpp>

#include <exception>

ft::serv::event_handler_base::event_handler_base()
{
}

ft::serv::event_handler_base::~event_handler_base()
{
}

void ft::serv::event_handler_base::on_active(event_layer& layer)
{
    layer.notify_active();
}

void ft::serv::event_handler_base::on_read(event_layer& layer, ft::shared_ptr<void> arg)
{
    layer.notify_read(arg);
}

void ft::serv::event_handler_base::on_read_complete(event_layer& layer)
{
    layer.notify_read_complete();
}

void ft::serv::event_handler_base::on_error(event_layer& layer, ft::shared_ptr<const std::exception> e)
{
    layer.notify_error(e);
}

void ft::serv::event_handler_base::on_inactive(event_layer& layer)
{
    layer.notify_inactive();
}

void ft::serv::event_handler_base::on_register(event_layer& layer)
{
    layer.post_register();
}

void ft::serv::event_handler_base::on_write(event_layer& layer, ft::shared_ptr<const void> arg)
{
    layer.post_write(arg);
}

void ft::serv::event_handler_base::on_flush(event_layer& layer)
{
    layer.post_flush();
}

void ft::serv::event_handler_base::on_finish(event_layer& layer)
{
    layer.post_finish();
}

void ft::serv::event_handler_base::on_disconnect(event_layer& layer)
{
    layer.post_disconnect();
}
