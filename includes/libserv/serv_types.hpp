/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#define FT_SERV_DEFAULT_LISTEN_BACKLOG 16
#define FT_SERV_DEFAULT_INIT_BUFFER_CAPACITY 4096
#define FT_SERV_MAX_EVENT_SIZE 4096
#define FT_SERV_WRITE_SPIN_COUNT 4

#define null NULL
#define staticassert(_x) static_cast<void>(sizeof(char[(_x) ? 1 : -1]))
#define beginof(_x) (&(_x)[0])
#define countof(_x) (sizeof(_x) / sizeof((_x)[0]))

#include <deque>
#include <map>
#include <set>
#include <vector>

namespace ft
{
    namespace serv
    {
        typedef int ident_t;

        typedef unsigned char byte_t;

        template <typename T>
        struct dynamic_array
        {
            typedef std::vector<T> type; // devector
        };

        template <typename K, typename V>
        struct fast_dictionary
        {
            typedef std::map<K, V> type; // flat_map
        };

        template <typename T>
        struct dynamic_buffer
        {
            typedef std::deque<T> type;
        };

        template <typename T>
        struct unique_set
        {
            typedef std::set<T> type; // unordered_set
        };
    }
}

#define foreach(_type, _name, _coll) for (_type _name = (_coll).begin(); _name != (_coll).end(); ++_name)
#define reverse_foreach(_type, _name, _coll) for (_type _name = (_coll).rbegin(); _name != (_coll).rend(); ++_name)

namespace ft
{
    namespace serv
    {
        namespace _internal
        {
            template <typename T>
            static inline void _lock(void* self_ptr)
            {
                static_cast<T*>(self_ptr)->lock();
            }

            template <typename T>
            static inline void _unlock(void* self_ptr)
            {
                static_cast<T*>(self_ptr)->unlock();
            }

            class synchronized_helper
            {
            private:
                void* lock;
                void (*fn_lock)(void*);
                void (*fn_unlock)(void*);

            public:
                template <typename T>
                synchronized_helper(T& lock) throw()
                    : lock(&lock),
                      fn_lock(&_lock<T>),
                      fn_unlock(&_unlock<T>)
                {
                    this->fn_lock(this->lock);
                }

                ~synchronized_helper()
                {
                    this->fn_unlock(this->lock);
                }

                inline operator bool() const throw()
                {
                    return false;
                }
            };
        }
    }
}

#define synchronized(_name)                                                            \
    if (const ft::serv::_internal::synchronized_helper __##__LINE__##helper = (_name)) \
        assert(false);                                                                 \
    else
