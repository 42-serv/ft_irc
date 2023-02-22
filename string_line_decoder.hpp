/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>

namespace ft
{
    namespace irc
    {
        class string_line_decoder : public ft::serv::bytes_decoder
        {
        protected:
            void decode(ft::serv::byte_buffer& buf, output_buffer& out)
            {
                const unsigned char* arr = buf.get();
                ft::serv::byte_buffer::size_type begin = 0;
                bool cr = false;
                for (ft::serv::byte_buffer::size_type i = 0; i < buf.size(); i++)
                {
                    if (cr && arr[i] == '\n')
                    {
                        out.push_back(ft::make_shared<std::string>(&arr[begin], &arr[i - 1]));
                        begin = i + 1;
                    }
                    else if (i - begin >= 512)
                    {
                        // FIXME: throw exceed 512 bytes without CR-LF
                    }

                    cr = arr[i] == '\r';
                }
                buf.remove(begin);
            }
        };

    }
}
