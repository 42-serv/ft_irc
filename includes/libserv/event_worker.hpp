/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "event_channel_base.hpp"
#include "serv_types.hpp"
#include "task_base.hpp"

#include <smart_ptr/shared_ptr.hpp>
#include <thread/condition_variable.hpp>
#include <thread/mutex.hpp>
#include <thread/thread.hpp>

#include <csignal>

namespace ft
{
    namespace serv
    {
        class event_worker
        {
        public:
            typedef fast_dictionary<ident_t, ft::shared_ptr<event_channel_base> >::type channel_dictionary;
            typedef dynamic_buffer<ft::shared_ptr<task_base> >::type task_list;

        private:
            ft::mutex lock;
            ft::condition_variable cond;
            bool active;
            ident_t loop_ident;
            ident_t event_ident;
            void* loop_list;
            channel_dictionary channels;
            task_list tasks;
            bool task_closed;
            ft::thread::id working_thread;

        public:
            event_worker();
            virtual ~event_worker();

            void add_channel(const ft::shared_ptr<event_channel_base>& channel);
            void remove_channel(const ident_t ident);
            void watch_ability(event_channel_base& channel);
            void loop();
            void wake_up() throw();
            void shutdown_loop() throw();

            void offer_task(const ft::shared_ptr<task_base>& task); // common
            void wait_for_loop();                                   // common
            bool is_in_event_loop() throw();                        // common

        private:
            event_worker(const event_worker&);
            event_worker& operator=(const event_worker&);

            void process_events(void* list, int n) throw();

            bool execute_tasks() throw(); // common
        };
    }
}
