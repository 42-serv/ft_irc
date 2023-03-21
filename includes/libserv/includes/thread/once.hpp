/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <pthread.h>

#include <cassert>

namespace ft
{
    const pthread_once_t pthread_once_init_value = PTHREAD_ONCE_INIT;

    class once_flag
    {
    public:
        typedef ::pthread_once_t* native_handle_type;
        typedef void (*native_routine_type)();

    private:
        ::pthread_once_t value;

    public:
        once_flag()
            : value(pthread_once_init_value) {}

        native_handle_type native_handle()
        {
            return &this->value;
        }

    private:
        once_flag(const once_flag&);
        once_flag& operator=(const once_flag&);
    };

    template <typename TFunc>
    void call_once(once_flag& flag, TFunc f)
    {
        assert(::pthread_once(flag.native_handle(), reinterpret_cast<once_flag::native_routine_type>(f)) == 0);
    }
}
