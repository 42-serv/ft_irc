/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "byte_buffer.hpp"
#include "event_handler_base.hpp"
#include "event_layer.hpp"
#include "serv_types.hpp"

#include <smart_ptr/shared_ptr.hpp>

namespace ft
{
    namespace serv
    {
        class bytes_decoder : public event_handler_base
        {
        public:
            typedef dynamic_buffer<ft::shared_ptr<void> >::type output_buffer;

        private:
            byte_buffer cumulative_buf;
            output_buffer output;

        public:
            bytes_decoder();
            ~bytes_decoder();

            void on_read(event_layer& layer, ft::shared_ptr<void>);
            void on_read_complete(event_layer& layer);

        protected:
            virtual void decode(byte_buffer& buf, output_buffer& out);

        private:
            bytes_decoder(const bytes_decoder&);
            bytes_decoder& operator=(const bytes_decoder&);
        };
    }
}
