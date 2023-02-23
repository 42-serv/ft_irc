/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "irc_constants.hpp"

#include "message.hpp"

#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>

namespace ft
{
    namespace irc
    {
        class message_decoder : public ft::serv::object_decoder
        {
        protected:
            void decode(cumulative_list& obj, output_buffer& out)
            {
                while (!obj.empty())
                {
                    ft::shared_ptr<std::string> line = ft::static_pointer_cast<std::string>(obj.front());
                    ft::shared_ptr<message> msg = ft::make_shared<message>();
                    obj.pop_front();
                    if (!message::try_parse(*line, *msg))
                    {
                        // FIXME: throw malformed message
                    }
                    out.push_back(msg);
                }
            }
        };
    }
}