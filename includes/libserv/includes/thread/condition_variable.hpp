/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "mutex.hpp"
#include "thread_error.hpp"

#include <pthread.h>
#include <sys/time.h> // struct timespec, struct timeval, gettimeofday()

#include <cassert>
#include <cerrno>
#include <ctime> // time_t

namespace ft
{
    class condition_variable
    {
    public:
        typedef ::pthread_cond_t* native_handle_type;
        typedef struct ::timespec timepoint_type;

    private:
        ::pthread_cond_t value;

    public:
        condition_variable()
        {
            const int e = ::pthread_cond_init(&this->value, NULL);
            if (e != 0)
            {
                throw thread_error(e, "pthread_cond_init");
            }
        }

        ~condition_variable()
        {
            assert(::pthread_cond_destroy(&this->value) == 0);
        }

        void notify_one() throw()
        {
            assert(::pthread_cond_signal(&this->value) == 0);
        }

        void notify_all() throw()
        {
            assert(::pthread_cond_broadcast(&this->value) == 0);
        }

        void wait(mutex& lock)
        {
            mutex::native_handle_type mutex = lock.native_handle();
            const int e = ::pthread_cond_wait(&this->value, mutex);
            if (e != 0)
            {
                throw thread_error(e, "pthread_cond_init");
            }
        }

        template <typename TPredicate>
        void wait(mutex& lock, TPredicate pred)
        {
            while (!pred())
            {
                this->wait(lock);
            }
        }

        bool wait_until(mutex& lock, const timepoint_type& abs_time)
        {
            mutex::native_handle_type mutex = lock.native_handle();
            const int e = ::pthread_cond_timedwait(&this->value, mutex, &abs_time);
            if (e != 0)
            {
                if (e == ETIMEDOUT)
                {
                    return false;
                }

                throw thread_error(e, "pthread_cond_timedwait");
            }

            return true;
        }

        template <typename TPredicate>
        bool wait_until(mutex& lock, const timepoint_type& abs_time, TPredicate pred)
        {
            while (!pred())
            {
                if (!this->wait_until(lock, abs_time))
                {
                    return false;
                }
            }
            return true;
        }

        bool wait_for(mutex& lock, std::time_t timeout, long nanos)
        {
            struct ::timeval tv;
            struct ::timespec ts;
            ::gettimeofday(&tv, NULL);
            ts.tv_sec = tv.tv_sec + timeout;
            ts.tv_nsec = nanos;
            return this->wait_until(lock, ts);
        }

        template <typename TPredicate>
        bool wait_for(mutex& lock, std::time_t timeout, long nanos, TPredicate pred)
        {
            struct ::timeval tv;
            struct ::timespec ts;
            ::gettimeofday(&tv, NULL);
            ts.tv_sec = tv.tv_sec + timeout;
            ts.tv_nsec = nanos;
            return this->wait_until(lock, ts, pred);
        }

        native_handle_type native_handle()
        {
            return &this->value;
        }

    private:
        condition_variable(const condition_variable&);
        condition_variable& operator=(const condition_variable&);
    };
}
