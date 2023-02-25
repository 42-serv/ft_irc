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
        class message_encoder : public ft::serv::object_encoder
        {
        protected:
            void encode(const ft::shared_ptr<const void>& out, output_buffer& target)
            {
                ft::shared_ptr<const ft::irc::message> message = ft::static_pointer_cast<const ft::irc::message>(out);
                target.push_back(ft::make_shared<std::string>(message->to_string()));
            }
        };
    }
}
