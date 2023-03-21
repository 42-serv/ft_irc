/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "event_handler_base.hpp"
#include "serv_types.hpp"

#include <smart_ptr/shared_ptr.hpp>
#include <smart_ptr/weak_ptr.hpp>

#include <exception>
#include <string>

namespace ft
{
    namespace serv
    {
        class event_channel_base;

        class event_worker;

        class task_base;

        class event_layer
        {
        private:
            event_channel_base& channel;
            ft::shared_ptr<event_handler_base> handler;
            ft::shared_ptr<event_layer> next;
            ft::weak_ptr<event_layer> prev;

        public:
            event_layer(event_channel_base& channel, const ft::shared_ptr<event_handler_base>& handler);
            ~event_layer();

        public:
            ident_t get_ident() const throw();
            const std::string& get_host() const throw();
            int get_serv() const throw();

            void set_next(const ft::shared_ptr<event_layer>& next);
            void set_prev(const ft::shared_ptr<event_layer>& prev);

            ft::shared_ptr<event_layer> shared_from_channel() throw();
            ft::shared_ptr<const event_layer> shared_from_channel() const throw();

        private:
            void on_active();
            void on_read(ft::shared_ptr<void>);
            void on_read_complete();
            void on_error(ft::shared_ptr<const std::exception>);
            void on_inactive();

            void do_register();
            void do_write(ft::shared_ptr<const void>);
            void do_flush();
            void do_finish();
            void do_disconnect();

        private:
            void invoke_on_active();
            void invoke_on_read(ft::shared_ptr<void>);
            void invoke_on_read_complete();
            void invoke_on_error(ft::shared_ptr<const std::exception>);
            void invoke_on_inactive();

            void invoke_do_register();
            void invoke_do_write(ft::shared_ptr<const void>);
            void invoke_do_flush();
            void invoke_do_finish();
            void invoke_do_disconnect();

        public:
            void notify_active();
            void notify_read(ft::shared_ptr<void>);
            void notify_read_complete();
            void notify_error(ft::shared_ptr<const std::exception>);
            void notify_inactive();

            void post_register();
            void post_write(ft::shared_ptr<const void>);
            void post_flush();
            void post_finish();
            void post_disconnect();

        public:
            void invoke_task(const ft::shared_ptr<task_base>& task) const;

        private:
            event_layer(const event_layer&);
            event_layer& operator=(const event_layer&);
        };
    }
}
