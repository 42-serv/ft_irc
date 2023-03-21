/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "thread_error.hpp"

#include <pthread.h>

#include <cassert>
#include <cerrno>

namespace ft
{
    class readwrite_lock
    {
    public:
        typedef ::pthread_rwlock_t* native_handle_type;

        class read_lock_type
        {
        private:
            readwrite_lock& rwlock;

        private:
            friend class readwrite_lock;
            read_lock_type(readwrite_lock& rwlock) : rwlock(rwlock) {}

        public:
            void lock() { this->rwlock.read_lock(); }
            bool try_lock() { return this->rwlock.try_read_lock(); }
            void unlock() { this->rwlock.unlock(); }
        };

        class write_lock_type
        {
        private:
            readwrite_lock& rwlock;

        private:
            friend class readwrite_lock;
            write_lock_type(readwrite_lock& rwlock) : rwlock(rwlock) {}

        public:
            void lock() { this->rwlock.write_lock(); }
            bool try_lock() { return this->rwlock.try_write_lock(); }
            void unlock() { this->rwlock.unlock(); }
        };

    private:
        ::pthread_rwlock_t value;
        read_lock_type read;
        write_lock_type write;

    public:
        readwrite_lock()
            : read(*this), write(*this)
        {
            const int e = ::pthread_rwlock_init(&this->value, NULL);
            if (e != 0)
            {
                throw thread_error(e, "pthread_rwlock_init");
            }
        }

        read_lock_type& get_read_lock()
        {
            return this->read;
        }

        write_lock_type& get_write_lock()
        {
            return this->write;
        }

        ~readwrite_lock()
        {
            assert(::pthread_rwlock_destroy(&this->value) == 0);
        }

        void read_lock()
        {
            const int e = ::pthread_rwlock_rdlock(&this->value);
            if (e != 0)
            {
                throw thread_error(e, "pthread_rwlock_rdlock");
            }
        }

        bool try_read_lock()
        {
            const int e = ::pthread_rwlock_tryrdlock(&this->value);
            if (e != 0)
            {
                if (e == EBUSY)
                {
                    return false;
                }

                throw thread_error(e, "pthread_rwlock_tryrdlock");
            }

            return true;
        }

        void write_lock()
        {
            const int e = ::pthread_rwlock_wrlock(&this->value);
            if (e != 0)
            {
                throw thread_error(e, "pthread_rwlock_wrlock");
            }
        }

        bool try_write_lock()
        {
            const int e = ::pthread_rwlock_trywrlock(&this->value);
            if (e != 0)
            {
                if (e == EBUSY)
                {
                    return false;
                }

                throw thread_error(e, "pthread_rwlock_trywrlock");
            }

            return true;
        }

        void unlock()
        {
            assert(::pthread_rwlock_unlock(&this->value) == 0);
        }

        native_handle_type native_handle()
        {
            return &this->value;
        }

    private:
        readwrite_lock(const readwrite_lock&);
        readwrite_lock& operator=(const readwrite_lock&);
    };
}
