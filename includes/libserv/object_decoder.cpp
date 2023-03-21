/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "object_decoder.hpp"

#include "event_layer.hpp"

#include <smart_ptr/shared_ptr.hpp>

#include <cassert>

ft::serv::object_decoder::object_decoder() : cumulative_obj()
{
}

ft::serv::object_decoder::~object_decoder()
{
}

void ft::serv::object_decoder::on_read(event_layer& layer, ft::shared_ptr<void> obj)
{
    this->cumulative_obj.push_back(obj);

    do
    {
        this->output.clear();
        const output_buffer::size_type size_prev = this->cumulative_obj.size();
        this->decode(this->cumulative_obj, this->output);
        if (this->cumulative_obj.size() == size_prev)
        {
            assert(this->output.empty());

            break;
        }

        assert(this->cumulative_obj.size() < size_prev);

        foreach (output_buffer::iterator, it, this->output)
        {
            layer.notify_read(*it);
        }
    } while (!0);
    this->output.clear();
}

void ft::serv::object_decoder::decode(cumulative_list&, output_buffer&)
{
    throw std::runtime_error("not implemented decode()");
}
