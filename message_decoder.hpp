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
                    ft::shared_ptr<ft::irc::message> message = ft::make_shared<ft::irc::message>();
                    obj.pop_front();
                    if (ft::irc::message::try_parse(*line, *message))
                    {
                        out.push_back(message);
                    }
                    else
                    {
                        // FIXME: throw malformed message
                    }
                }
            }
        };
    }
}
