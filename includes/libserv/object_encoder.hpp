/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "event_handler_base.hpp"
#include "event_layer.hpp"
#include "serv_types.hpp"

#include <smart_ptr/shared_ptr.hpp>

namespace ft
{
    namespace serv
    {
        class object_encoder : public event_handler_base
        {
        public:
            typedef dynamic_buffer<ft::shared_ptr<const void> >::type output_buffer;

        private:
            output_buffer temp;

        public:
            object_encoder();
            virtual ~object_encoder();

            virtual void on_write(event_layer& layer, ft::shared_ptr<const void>);

        protected:
            virtual void encode(const ft::shared_ptr<const void>& out, output_buffer& target);

        private:
            object_encoder(const object_encoder&);
            object_encoder& operator=(const object_encoder&);
        };
    }
}
