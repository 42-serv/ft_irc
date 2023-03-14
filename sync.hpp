#include <libserv/libserv.hpp>
#include <thread/readwrite_lock.hpp>

#pragma once

namespace ft
{
    namespace irc
    {
        template <typename T>
        class sync
        {
        private:
            ft::readwrite_lock& lock;
            T value;

        public:
            sync(ft::readwrite_lock& lock, T init_value = T()) : lock(lock), value(init_value){};

            const T& get() const throw() { return this->value; }

            T load() const
            {
                synchronized (lock.get_read_lock())
                {
                    return this->get();
                }
            }

            void set(const T& value)
            {
                this->value = value;
            }

            void store(const T& value)
            {
                synchronized (lock.get_write_lock())
                {
                    this->set(value);
                }
            }
        };
    }
}
