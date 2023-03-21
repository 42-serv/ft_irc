/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "event_worker_group.hpp"

#include "event_channel_base.hpp"
#include "event_worker.hpp"
#include "serv_types.hpp"

#include <smart_ptr/make_shared.hpp>
#include <smart_ptr/shared_ptr.hpp>
#include <thread/mutex.hpp>
#include <thread/thread.hpp>

ft::serv::event_worker_group::event_worker_group()
    : lock(),
      index(),
      loops(),
      threads()
{
}

ft::serv::event_worker_group::~event_worker_group()
{
}

static void* _run_loop(void* arg)
{
    ft::serv::event_worker& loop = *static_cast<ft::serv::event_worker*>(arg);
    loop.loop();
    return null;
}

void ft::serv::event_worker_group::put_worker(const ft::shared_ptr<event_worker>& worker)
{
    synchronized (this->lock)
    {
        const ft::shared_ptr<ft::thread> working_thread = ft::make_shared<ft::thread>();
        working_thread->start(&_run_loop, worker.get());
        this->threads.push_back(working_thread);
        this->loops.push_back(worker);
    }
}

void ft::serv::event_worker_group::wait_all()
{
    assert(!this->loops.empty());

    foreach (loop_list::iterator, it, this->loops)
    {
        const ft::shared_ptr<event_worker>& lp = *it;
        lp->wait_for_loop();
    }
}

void ft::serv::event_worker_group::shutdown_all() throw()
{
    assert(!this->loops.empty());

    foreach (loop_list::iterator, it, this->loops)
    {
        const ft::shared_ptr<event_worker>& lp = *it;
        lp->shutdown_loop();
    }
}

void ft::serv::event_worker_group::join_all()
{
    assert(!this->threads.empty());

    foreach (thread_list::iterator, it, this->threads)
    {
        const ft::shared_ptr<ft::thread>& th = *it;
        th->join();
    }
}

const ft::shared_ptr<ft::serv::event_worker>& ft::serv::event_worker_group::next()
{
    assert(!this->loops.empty());

    synchronized (this->lock)
    {
        return this->loops[this->index++ % this->loops.size()];
    }
}
