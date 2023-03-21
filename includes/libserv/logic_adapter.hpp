/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "event_handler_base.hpp"
#include "event_layer.hpp"

#include <smart_ptr/shared_ptr.hpp>

namespace ft
{
    namespace serv
    {
        class logic_adapter : public event_handler_base
        {
        public:
            logic_adapter();
            virtual ~logic_adapter();

            virtual void on_active(event_layer& layer);
            virtual void on_read(event_layer& layer, ft::shared_ptr<void>);
            virtual void on_read_complete(event_layer& layer);
            virtual void on_error(event_layer& layer, ft::shared_ptr<const std::exception>);
            virtual void on_inactive(event_layer& layer);

        private:
            logic_adapter(const logic_adapter&);
            logic_adapter& operator=(const logic_adapter&);
        };
    }
}
