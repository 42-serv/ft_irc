/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "logic_adapter.hpp"

#include "event_layer.hpp"

#include <smart_ptr/shared_ptr.hpp>

ft::serv::logic_adapter::logic_adapter()
{
}

ft::serv::logic_adapter::~logic_adapter()
{
}

void ft::serv::logic_adapter::on_active(event_layer& layer)
{
    static_cast<void>(layer);
}

void ft::serv::logic_adapter::on_read(event_layer& layer, ft::shared_ptr<void>)
{
    static_cast<void>(layer);
}

void ft::serv::logic_adapter::on_read_complete(event_layer& layer)
{
    static_cast<void>(layer);
}

void ft::serv::logic_adapter::on_error(event_layer& layer, ft::shared_ptr<const std::exception>)
{
    static_cast<void>(layer);
}

void ft::serv::logic_adapter::on_inactive(event_layer& layer)
{
    static_cast<void>(layer);
}
