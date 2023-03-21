/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "event_layer.hpp"

#include "event_channel_base.hpp"
#include "event_handler_base.hpp"
#include "event_worker.hpp"
#include "serv_types.hpp"
#include "task_base.hpp"

#include <smart_ptr/make_shared.hpp>
#include <smart_ptr/shared_ptr.hpp>
#include <smart_ptr/weak_ptr.hpp>

#include <exception>

ft::serv::event_layer::event_layer(event_channel_base& channel, const ft::shared_ptr<event_handler_base>& handler)
    : channel(channel),
      handler(handler),
      next(),
      prev()
{
}

ft::serv::event_layer::~event_layer()
{
}

ft::serv::ident_t ft::serv::event_layer::get_ident() const throw()
{
    return this->channel.get_ident();
}

const std::string& ft::serv::event_layer::get_host() const throw()
{
    return this->channel.get_host();
}

int ft::serv::event_layer::get_serv() const throw()
{
    return this->channel.get_serv();
}

void ft::serv::event_layer::set_next(const ft::shared_ptr<event_layer>& next)
{
    if (this->next)
    {
        next->next = this->next;
        next->prev = this->next->prev;
        this->next->prev = next;
    }
    this->next = next;
}

void ft::serv::event_layer::set_prev(const ft::shared_ptr<event_layer>& prev)
{
    if (!this->prev.expired())
    {
        const ft::shared_ptr<event_layer> prev_old = this->prev.lock();
        prev->prev = prev_old;
        prev->next = prev_old->next;
        prev_old->next = prev;
    }
    this->prev = prev;
}

ft::shared_ptr<ft::serv::event_layer> ft::serv::event_layer::shared_from_channel() throw()
{
    return ft::shared_ptr<ft::serv::event_layer>(this->channel.shared_from_this(), this);
}

ft::shared_ptr<const ft::serv::event_layer> ft::serv::event_layer::shared_from_channel() const throw()
{
    return ft::shared_ptr<const ft::serv::event_layer>(this->channel.shared_from_this(), this);
}

void ft::serv::event_layer::on_active()
{
    this->handler->on_active(*this);
}

void ft::serv::event_layer::on_read(ft::shared_ptr<void> arg)
{
    this->handler->on_read(*this, arg);
}

void ft::serv::event_layer::on_read_complete()
{
    this->handler->on_read_complete(*this);
}

void ft::serv::event_layer::on_error(ft::shared_ptr<const std::exception> e)
{
    this->handler->on_error(*this, e);
}

void ft::serv::event_layer::on_inactive()
{
    this->handler->on_inactive(*this);
}

void ft::serv::event_layer::do_register()
{
    this->handler->on_register(*this);
}

void ft::serv::event_layer::do_write(ft::shared_ptr<const void> arg)
{
    this->handler->on_write(*this, arg);
}

void ft::serv::event_layer::do_flush()
{
    this->handler->on_flush(*this);
}

void ft::serv::event_layer::do_finish()
{
    this->handler->on_finish(*this);
}

void ft::serv::event_layer::do_disconnect()
{
    this->handler->on_disconnect(*this);
}

typedef void (ft::serv::event_layer::*member_function_pointer)(void);
FT_SERV_DEFINE_TASK_3(event_layer_task,
                      ft::serv::event_layer&, self,
                      ft::shared_ptr<ft::serv::event_channel_base>, _hold,
                      member_function_pointer, fn,
                      (self.*fn)());

typedef void (ft::serv::event_layer::*member_function_pointer_pv)(ft::shared_ptr<void>);
FT_SERV_DEFINE_TASK_4(event_layer_task_pv,
                      ft::serv::event_layer&, self,
                      ft::shared_ptr<ft::serv::event_channel_base>, _hold,
                      member_function_pointer_pv, fn,
                      ft::shared_ptr<void>, pv,
                      (self.*fn)(pv));

typedef void (ft::serv::event_layer::*member_function_pointer_pcv)(ft::shared_ptr<const void>);
FT_SERV_DEFINE_TASK_4(event_layer_task_pcv,
                      ft::serv::event_layer&, self,
                      ft::shared_ptr<ft::serv::event_channel_base>, _hold,
                      member_function_pointer_pcv, fn,
                      ft::shared_ptr<const void>, pcv,
                      (self.*fn)(pcv));

typedef void (ft::serv::event_layer::*member_function_pointer_e)(ft::shared_ptr<const std::exception>);
FT_SERV_DEFINE_TASK_4(event_layer_task_e,
                      ft::serv::event_layer&, self,
                      ft::shared_ptr<ft::serv::event_channel_base>, _hold,
                      member_function_pointer_e, fn,
                      ft::shared_ptr<const std::exception>, e,
                      (self.*fn)(e));

void ft::serv::event_layer::invoke_on_active()
{
    const ft::shared_ptr<event_worker> loop = this->channel.get_loop();

    if (loop->is_in_event_loop())
    {
        this->on_active();
    }
    else
    {
        loop->offer_task(ft::make_shared<event_layer_task>(*this, this->channel.shared_from_this(), &ft::serv::event_layer::on_active));
    }
}

void ft::serv::event_layer::invoke_on_read(ft::shared_ptr<void> arg)
{
    const ft::shared_ptr<event_worker> loop = this->channel.get_loop();

    if (loop->is_in_event_loop())
    {
        this->on_read(arg);
    }
    else
    {
        loop->offer_task(ft::make_shared<event_layer_task_pv>(*this, this->channel.shared_from_this(), &ft::serv::event_layer::on_read, arg));
    }
}

void ft::serv::event_layer::invoke_on_read_complete()
{
    const ft::shared_ptr<event_worker> loop = this->channel.get_loop();

    if (loop->is_in_event_loop())
    {
        this->on_read_complete();
    }
    else
    {
        loop->offer_task(ft::make_shared<event_layer_task>(*this, this->channel.shared_from_this(), &ft::serv::event_layer::on_read_complete));
    }
}

void ft::serv::event_layer::invoke_on_error(ft::shared_ptr<const std::exception> e)
{
    const ft::shared_ptr<event_worker> loop = this->channel.get_loop();

    if (loop->is_in_event_loop())
    {
        this->on_error(e);
    }
    else
    {
        loop->offer_task(ft::make_shared<event_layer_task_e>(*this, this->channel.shared_from_this(), &ft::serv::event_layer::on_error, e));
    }
}

void ft::serv::event_layer::invoke_on_inactive()
{
    const ft::shared_ptr<event_worker> loop = this->channel.get_loop();

    // SPECIAL

    // if (loop->is_in_event_loop())
    // {
    //     this->on_inactive();
    // }
    // else
    // {
    loop->offer_task(ft::make_shared<event_layer_task>(*this, this->channel.shared_from_this(), &ft::serv::event_layer::on_inactive));
    // }
}

void ft::serv::event_layer::invoke_do_register()
{
    const ft::shared_ptr<event_worker> loop = this->channel.get_loop();

    if (loop->is_in_event_loop())
    {
        this->do_register();
    }
    else
    {
        loop->offer_task(ft::make_shared<event_layer_task>(*this, this->channel.shared_from_this(), &ft::serv::event_layer::do_register));
    }
}

void ft::serv::event_layer::invoke_do_write(ft::shared_ptr<const void> arg)
{
    const ft::shared_ptr<event_worker> loop = this->channel.get_loop();

    if (loop->is_in_event_loop())
    {
        this->do_write(arg);
    }
    else
    {
        loop->offer_task(ft::make_shared<event_layer_task_pcv>(*this, this->channel.shared_from_this(), &ft::serv::event_layer::do_write, arg));
    }
}

void ft::serv::event_layer::invoke_do_flush()
{
    const ft::shared_ptr<event_worker> loop = this->channel.get_loop();

    if (loop->is_in_event_loop())
    {
        this->do_flush();
    }
    else
    {
        loop->offer_task(ft::make_shared<event_layer_task>(*this, this->channel.shared_from_this(), &ft::serv::event_layer::do_flush));
    }
}

void ft::serv::event_layer::invoke_do_finish()
{
    const ft::shared_ptr<event_worker> loop = this->channel.get_loop();

    if (loop->is_in_event_loop())
    {
        this->do_finish();
    }
    else
    {
        loop->offer_task(ft::make_shared<event_layer_task>(*this, this->channel.shared_from_this(), &ft::serv::event_layer::do_finish));
    }
}

void ft::serv::event_layer::invoke_do_disconnect()
{
    const ft::shared_ptr<event_worker> loop = this->channel.get_loop();

    // SPECIAL

    // if (loop->is_in_event_loop())
    // {
    //     this->do_disconnect();
    // }
    // else
    // {
    loop->offer_task(ft::make_shared<event_layer_task>(*this, this->channel.shared_from_this(), &ft::serv::event_layer::do_disconnect));
    // }
}

void ft::serv::event_layer::notify_active()
{
    this->next->invoke_on_active();
}

void ft::serv::event_layer::notify_read(ft::shared_ptr<void> arg)
{
    this->next->invoke_on_read(arg);
}

void ft::serv::event_layer::notify_read_complete()
{
    this->next->invoke_on_read_complete();
}

void ft::serv::event_layer::notify_error(ft::shared_ptr<const std::exception> e)
{
    this->next->invoke_on_error(e);
}

void ft::serv::event_layer::notify_inactive()
{
    this->next->invoke_on_inactive();
}

void ft::serv::event_layer::post_register()
{
    this->prev.lock()->invoke_do_register();
}

void ft::serv::event_layer::post_write(ft::shared_ptr<const void> arg)
{
    this->prev.lock()->invoke_do_write(arg);
}

void ft::serv::event_layer::post_flush()
{
    this->prev.lock()->invoke_do_flush();
}

void ft::serv::event_layer::post_finish()
{
    this->prev.lock()->invoke_do_finish();
}

void ft::serv::event_layer::post_disconnect()
{
    this->prev.lock()->invoke_do_disconnect();
}

void ft::serv::event_layer::invoke_task(const ft::shared_ptr<task_base>& task) const
{
    const ft::shared_ptr<event_worker> loop = this->channel.get_loop();

    loop->offer_task(task);
}
