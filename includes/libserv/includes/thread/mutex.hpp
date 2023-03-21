/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "thread_error.hpp"

#include <pthread.h>

#include <cassert>
#include <cerrno>

namespace ft
{
    class mutex
    {
    public:
        typedef ::pthread_mutex_t* native_handle_type;

    private:
        ::pthread_mutex_t value;

    public:
        mutex()
        {
            const int e = ::pthread_mutex_init(&this->value, NULL);
            if (e != 0)
            {
                throw thread_error(e, "pthread_mutex_init");
            }
        }

        ~mutex()
        {
            assert(::pthread_mutex_destroy(&this->value) == 0);
        }

        void lock()
        {
            const int e = ::pthread_mutex_lock(&this->value);
            if (e != 0)
            {
                throw thread_error(e, "pthread_mutex_lock");
            }
        }

        bool try_lock()
        {
            const int e = ::pthread_mutex_trylock(&this->value);
            if (e != 0)
            {
                if (e == EBUSY)
                {
                    return false;
                }

                throw thread_error(e, "pthread_mutex_trylock");
            }

            return true;
        }

        void unlock()
        {
            assert(::pthread_mutex_unlock(&this->value) == 0);
        }

        native_handle_type native_handle()
        {
            return &this->value;
        }

    private:
        mutex(const mutex&);
        mutex& operator=(const mutex&);
    };
}
