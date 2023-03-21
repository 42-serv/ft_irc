/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "thread_error.hpp"

#include <pthread.h>

#include <cassert>
#include <stdexcept>

namespace ft
{
    class thread
    {
    public:
        class id
        {
        public:
            typedef ::pthread_t native_type;

        private:
            bool init;
            native_type thread;

        public:
            id()
                : init(false) {}

            id(native_type thread)
                : init(true), thread(thread) {}

            id(const id& that) : init(that.init), thread(that.thread) {}

            ~id() {}

            id& operator=(const id& that)
            {
                if (this != &that)
                {
                    this->init = that.init;
                    this->thread = that.thread;
                }
                return *this;
            }

            bool is_init() const throw()
            {
                return this->init;
            }

            native_type get() const throw()
            {
                return this->thread;
            }

            void set(const native_type& thread) throw()
            {
                this->init = true;
                this->thread = thread;
            }

            void reset() throw()
            {
                this->init = false;
            }

            void swap(id& that) throw()
            {
                bool b = this->init;
                this->init = that.init;
                that.init = b;

                native_type t = this->thread;
                this->thread = that.thread;
                that.thread = t;
            }

            // explicit operator bool
            void unspecified_bool_type_func() const {}
            typedef void (thread::id::*unspecified_bool_type)() const;
            operator unspecified_bool_type() const throw()
            {
                return !this->is_init() ? NULL : &thread::id::unspecified_bool_type_func;
            }

        public:
            friend bool operator==(const id& lhs, const id& rhs)
            {
                if (!lhs.init && !rhs.init)
                {
                    return true;
                }

                return lhs.init == rhs.init && ::pthread_equal(lhs.thread, rhs.thread) != 0;
            }

            friend bool operator!=(const id& lhs, const id& rhs)
            {
                return !(lhs == rhs);
            }
        };

        static inline id self()
        {
            return ::pthread_self();
        }

        static inline void exit(void* status)
        {
            ::pthread_exit(status);
        }

    private:
        id key;

    public:
        thread() throw() {}

        ~thread()
        {
            assert(!this->joinable());
        }

        thread& start(void* (*f)(void*), void* arg)
        {
            if (this->key.is_init())
            {
                throw std::runtime_error("start");
            }

            id::native_type thread;
            const int e = ::pthread_create(&thread, NULL, f, arg);
            if (e != 0)
            {
                throw thread_error(e, "pthread_create");
            }
            this->key.set(thread);

            return *this;
        }

        bool joinable() throw()
        {
            return this->key.is_init();
        }

        void* join()
        {
            if (!this->key.is_init())
            {
                throw std::runtime_error("join");
            }

            void* result;
            const int e = ::pthread_join(this->key.get(), &result);
            this->key.reset();
            if (e != 0)
            {
                throw thread_error(e, "pthread_join");
            }
            return result;
        }

        void detach()
        {
            id local_key;
            this->key.swap(local_key);

            if (local_key.is_init())
            {
                assert(::pthread_detach(local_key.get()) == 0);
            }
        }

        id get_id() const throw()
        {
            return this->key;
        }

        void swap(thread& that) throw()
        {
            this->key.swap(that.key);
        }

    private:
        thread(const thread&);
        thread& operator=(const thread& that);
    };
}
