/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "serv_types.hpp"
#include "stream_channel.hpp"

#include <cstddef>
#include <string>

namespace ft
{
    namespace serv
    {
        class file_channel : public stream_channel
        {
        private:
            std::size_t init_buf_capacity;

        public:
            file_channel(ident_t ident, const std::string& host, int serv, std::size_t init_buf_capacity = FT_SERV_DEFAULT_INIT_BUFFER_CAPACITY);
            virtual ~file_channel();

        protected:
            virtual long do_read(void* const buf, const std::size_t len) throw();
            virtual long do_write(const void* const buf, const std::size_t len) throw();

        private:
            file_channel(const file_channel&);
            file_channel& operator=(const file_channel&);
        };
    }
}
