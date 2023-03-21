/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "serv_types.hpp"

#include <smart_ptr/shared_ptr.hpp>

#include <exception>

namespace ft
{
    namespace serv
    {
        class event_layer;

        class event_handler_base
        {
        public:
            event_handler_base();
            virtual ~event_handler_base();

            virtual void on_active(event_layer& layer);
            virtual void on_read(event_layer& layer, ft::shared_ptr<void>);
            virtual void on_read_complete(event_layer& layer);
            virtual void on_error(event_layer& layer, ft::shared_ptr<const std::exception>);
            virtual void on_inactive(event_layer& layer);

            virtual void on_register(event_layer& layer);
            virtual void on_write(event_layer& layer, ft::shared_ptr<const void>);
            virtual void on_flush(event_layer& layer);
            virtual void on_finish(event_layer& layer);
            virtual void on_disconnect(event_layer& layer);

        private:
            event_handler_base(const event_handler_base&);
            event_handler_base& operator=(const event_handler_base&);
        };
    }
}
