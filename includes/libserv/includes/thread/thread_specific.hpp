/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <pthread.h>

#include <cassert>

namespace ft
{
    template <typename T>
    class thread_specific_ptr
    {
    private:
        static void default_deleter(T* ptr)
        {
            delete ptr;
        }

    public:
        typedef T element_type;
        typedef void (*deleter_type)(T*);
        typedef ::pthread_key_t* native_handle_type;
        typedef void (*native_deleter_type)(void*);

    private:
        ::pthread_key_t key;
        deleter_type deleter;

    public:
        explicit thread_specific_ptr(deleter_type deleter = &default_deleter)
            : deleter(deleter)
        {
            assert(::pthread_key_create(&this->key, reinterpret_cast<native_deleter_type>(deleter)) == 0);
        }

        ~thread_specific_ptr()
        {
            assert(::pthread_key_delete(this->key) == 0);
        }

        T* get() const
        {
            return static_cast<T*>(::pthread_getspecific(this->key));
        }

        T* operator->() const
        {
            return this->get();
        }

        T& operator*() const
        {
            return *this->get();
        }

        T* release()
        {
            T* const value = this->get();
            assert(::pthread_setspecific(this->key, NULL) == 0);
            return value;
        }

        void reset(T* new_value = NULL)
        {
            T* const value = this->get();
            if (value != new_value)
            {
                (*this->deleter)(value);
                assert(::pthread_setspecific(this->key, new_value) == 0);
            }
        }

        native_handle_type native_handle()
        {
            return &this->key;
        }

    private:
        thread_specific_ptr(thread_specific_ptr&);
        thread_specific_ptr& operator=(thread_specific_ptr&);
    };
}
