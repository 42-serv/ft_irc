/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "event_worker.hpp"

#include "event_channel_base.hpp"
#include "logger.hpp"
#include "serv_exception.hpp"
#include "serv_types.hpp"

#include <smart_ptr/shared_ptr.hpp>
#include <thread/condition_variable.hpp>
#include <thread/mutex.hpp>
#include <thread/thread.hpp>

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <exception>
#include <map>
#include <string>
#include <vector>

namespace ft
{
    namespace serv
    {
        typedef dynamic_array<struct ::epoll_event>::type event_list;

        static const event_list::size_type MAX_EVENTS = FT_SERV_MAX_EVENT_SIZE;
        static const ::eventfd_t EVFD_NORMAL = 1 << 1;
        static const ::eventfd_t EVFD_SHUTDOWN = 1;

        static void _epoll_operation(ident_t epoll_fd, int epoll_operation, event_channel_base& channel) throw()
        {
            bool interested[2];
            bool changed[2];
            channel.load_interested(interested, changed);
            int flags = 0;
            if (interested[0])
            {
                flags |= EPOLLIN;
            }
            if (interested[1])
            {
                flags |= EPOLLOUT;
            }
            const ident_t ident = channel.get_ident();
            struct ::epoll_event change;
            change.events = flags;
            std::memset(&change.data, 0, sizeof(change.data));
            change.data.fd = ident;
            if (::epoll_ctl(epoll_fd, epoll_operation, ident, &change) < 0)
            {
                const syscall_failed e;
                static_cast<void>(e); // ignore
            }
            channel.store_interested();
        }

        static void _write_eventfd(ident_t event_ident, ::eventfd_t value) throw()
        {
            const int r = ::eventfd_write(event_ident, value);
            static_cast<void>(r); // ignore errors
        }
    }
}

ft::serv::event_worker::event_worker()
    : lock(),
      cond(),
      active(),
      loop_ident(::epoll_create1(EPOLL_CLOEXEC)),
      event_ident(0),
      loop_list(),
      channels(),
      tasks(),
      task_closed(),
      working_thread()
{
    try
    {
        if (this->loop_ident < 0)
        {
            throw syscall_failed();
        }

        this->event_ident = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
        if (this->event_ident < 0)
        {
            throw syscall_failed();
        }

        struct ::epoll_event change;
        change.events = EPOLLIN;
        std::memset(&change.data, 0, sizeof(change.data));
        change.data.fd = this->event_ident;
        if (::epoll_ctl(this->loop_ident, EPOLL_CTL_ADD, this->event_ident, &change) < 0)
        {
            throw syscall_failed();
        }
    }
    catch (const syscall_failed& e)
    {
        if (!(this->loop_ident < 0))
        {
            ::close(this->loop_ident);
        }
        if (!(this->event_ident < 0))
        {
            ::close(this->event_ident);
        }
        throw;
    }
    logger::trace("Create Event Worker (%d)", this->loop_ident);
}

ft::serv::event_worker::~event_worker()
{
    logger::trace("Destroy Event Worker (%d)", this->loop_ident);
    ::close(this->loop_ident);
    ::close(this->event_ident);
}

void ft::serv::event_worker::add_channel(const ft::shared_ptr<event_channel_base>& channel)
{
    assert(this->is_in_event_loop());

    const ident_t ident = channel->get_ident();

    const bool success = this->channels.insert(std::make_pair(ident, channel)).second;
    assert(success); // NOTE: duplicate identity

    logger::trace("Event Worker (%d): Add Event Channel (%d)", this->loop_ident, ident);
    _epoll_operation(this->loop_ident, EPOLL_CTL_ADD, *channel);
}

void ft::serv::event_worker::remove_channel(const ident_t ident)
{
    assert(this->is_in_event_loop());

    channel_dictionary::/*const_*/ iterator it = this->channels.find(ident);

    if (it != this->channels.end())
    {
        const ft::shared_ptr<event_channel_base>& channel = it->second;
        _epoll_operation(this->loop_ident, EPOLL_CTL_DEL, *channel);
        logger::trace("Event Worker (%d): Remove Event Channel (%d): Success", this->loop_ident, ident);
        this->channels.erase(it);
    }
    else
    {
        // ignore
        logger::trace("Event Worker (%d): Remove Event Channel (%d): Not Found", this->loop_ident, ident);
    }
}

void ft::serv::event_worker::watch_ability(event_channel_base& channel)
{
    assert(this->is_in_event_loop());

    const ident_t ident = channel.get_ident();
    _epoll_operation(this->loop_ident, EPOLL_CTL_MOD, channel);
    logger::trace("Event Worker (%d): Watch Ability Changed (%d): R=%d, W=%d", this->loop_ident, ident, channel.is_readability_enabled(), channel.is_writability_enabled());
}

void ft::serv::event_worker::loop()
{
    this->working_thread = ft::thread::self();
    synchronized (this->lock)
    {
        this->active = true;
    }
    this->cond.notify_all();

    event_list events;

    events.resize(MAX_EVENTS);

    this->loop_list = null;

    for (;;)
    {
        ::time_t timeout_millis;
        synchronized (this->lock)
        {
            // if has task then polling
            timeout_millis = this->tasks.empty() ? 1000 : 0;
        }
        const int n = ::epoll_wait(this->loop_ident, events.data(), events.size(), timeout_millis);
        if (n < 0)
        {
            const syscall_failed e;
            if (e.error() == EINTR)
            {
                // ignore
            }
            else
            {
                logger::warn("Event Worker (%d): Error on epoll_wait: %s", this->loop_ident, e.what());
                ::sleep(1);
            }
            continue;
        }

        if (n != 0)
        {
            this->process_events(&events, n);
        }

        if (!this->execute_tasks())
        {
            break;
        }

        if (static_cast<event_list::size_type>(n) == events.size())
        {
            // double
            events.resize(n << 1);
        }
    }
}

void ft::serv::event_worker::wake_up() throw()
{
    if (!this->is_in_event_loop())
    {
        _write_eventfd(this->event_ident, EVFD_NORMAL);
    }
}

void ft::serv::event_worker::shutdown_loop() throw()
{
    _write_eventfd(this->event_ident, EVFD_SHUTDOWN);
}

void ft::serv::event_worker::process_events(void* list, int n) throw()
{
    event_list& events = *static_cast<event_list*>(list);

    for (int i = 0; i < n; i++)
    {
        const struct ::epoll_event& evi = events[i];

        const ident_t ident = evi.data.fd;
        if (ident == this->event_ident)
        {
            ::eventfd_t value;
            const int r = ::eventfd_read(ident, &value);
            static_cast<void>(r); // ignore errors
            logger::trace("Event Worker (%d): Epoll Wakeup Event: value %d", this->loop_ident, value);
            if (value & EVFD_SHUTDOWN)
            {
                synchronized (this->lock)
                {
                    this->active = false;
                }
                break;
            }
            continue;
        }

        const channel_dictionary::const_iterator it_channel = this->channels.find(ident);
        if (it_channel == this->channels.end())
        {
            const int r = ::epoll_ctl(this->loop_ident, EPOLL_CTL_DEL, ident, null);
            static_cast<void>(r); // ignore errors
            logger::warn("Event Worker (%d): Channel Not Found (%d)", this->loop_ident, ident);
            continue;
        }
        ft::shared_ptr<event_channel_base> channel = it_channel->second; // lock ref count

        logger::trace("Event Worker (%d): Epoll Event (%d): %x", this->loop_ident, ident, evi.events);
        if (evi.events & (EPOLLOUT | EPOLLERR))
        {
            channel->trigger_write();
        }

        if (evi.events & (EPOLLIN | EPOLLERR))
        {
            channel->trigger_read();
        }
    }
}
