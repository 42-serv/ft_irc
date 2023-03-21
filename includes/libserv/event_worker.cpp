#include "event_worker.hpp"

#include <thread/condition_variable.hpp>

#include <cassert>
#include <exception>

void ft::serv::event_worker::offer_task(const ft::shared_ptr<task_base>& task)
{
    synchronized (this->lock)
    {
        if (this->task_closed)
        {
            // rejected
            return;
        }

        try
        {
            this->tasks.push_back(task);
            this->wake_up();
        }
        catch (const std::exception& e)
        {
            // rejected
        }
    }
}

void ft::serv::event_worker::wait_for_loop()
{
    synchronized (this->lock)
    {
        if (!this->active)
        {
            this->cond.wait(this->lock);
        }
    }
}

bool ft::serv::event_worker::is_in_event_loop() throw()
{
    assert(this->working_thread);

    return ft::thread::self() == this->working_thread;
}

bool ft::serv::event_worker::execute_tasks() throw()
{
    bool in_progress;

    task_list snapshot;
    synchronized (this->lock)
    {
        this->tasks.swap(snapshot);

        in_progress = this->active;
        if (!in_progress)
        {
            this->task_closed = true;
        }
    }

    foreach (task_list::iterator, it, snapshot)
    {
        const ft::shared_ptr<task_base>& task = *it;
        task->run();
    }

    return in_progress;
}
