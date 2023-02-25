/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include "message.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>

#include <string>

namespace ft
{
    namespace irc
    {
        class string_line_encoder : public ft::serv::bytes_encoder
        {
        protected:
            void encode(const ft::shared_ptr<const void>& out, ft::serv::byte_buffer& target)
            {
                ft::shared_ptr<const std::string> line = ft::static_pointer_cast<const std::string>(out);
                target.put(line->data(), line->size());
                target.put("\r\n", sizeof("\r\n") - 1);
            }
        };
    }
}
