/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <pthread.h>

#include <cassert>

// #include <iostream>
// #define OUTPUT_REF_COUNTED std::cout

namespace ft
{
    namespace _internal
    {
        class _counted_base
        {
        private:
            typedef signed int count_type;

            count_type shared_count;
            count_type weak_count;
            mutable pthread_mutex_t mutex;

            _counted_base(const _counted_base&);
            _counted_base& operator=(const _counted_base&);

        public:
            _counted_base()
                : shared_count(1), weak_count(1)
            {
                assert(pthread_mutex_init(&this->mutex, 0) == 0);
            }

            virtual ~_counted_base() // throw()
            {
                assert(pthread_mutex_destroy(&this->mutex) == 0);
            }

            virtual void dispose() = 0; // throw()
            virtual void destroy() = 0; // throw()

            void add_ref_copy()
            {
                assert(pthread_mutex_lock(&this->mutex) == 0);
#ifdef OUTPUT_REF_COUNTED
                OUTPUT_REF_COUNTED << static_cast<const void*>(this) << ": " << __PRETTY_FUNCTION__ << ": ++" << this->shared_count << " (Weak=" << this->weak_count << ")" << std::endl;
#endif
                ++this->shared_count;
                assert(pthread_mutex_unlock(&this->mutex) == 0);
            }

            bool add_ref_lock()
            {
                assert(pthread_mutex_lock(&this->mutex) == 0);
#ifdef OUTPUT_REF_COUNTED
                OUTPUT_REF_COUNTED << static_cast<const void*>(this) << ": " << __PRETTY_FUNCTION__ << ": ++" << this->shared_count << " (Weak=" << this->weak_count << ")" << std::endl;
#endif
                bool success = this->shared_count == 0 ? false : (static_cast<void>(++this->shared_count), true);
                assert(pthread_mutex_unlock(&this->mutex) == 0);
                return success;
            }

            void release() // throw()
            {
                assert(pthread_mutex_lock(&this->mutex) == 0);
#ifdef OUTPUT_REF_COUNTED
                OUTPUT_REF_COUNTED << static_cast<const void*>(this) << ": " << __PRETTY_FUNCTION__ << ": --" << this->shared_count << " (Weak=" << this->weak_count << ")" << std::endl;
#endif
                bool release_resource = --this->shared_count == 0;
                assert(pthread_mutex_unlock(&this->mutex) == 0);

                if (release_resource)
                {
                    this->dispose();
                    this->weak_release();
                }
            }

            void weak_add_ref() // throw()
            {
                assert(pthread_mutex_lock(&this->mutex) == 0);
#ifdef OUTPUT_REF_COUNTED
                OUTPUT_REF_COUNTED << static_cast<const void*>(this) << ": " << __PRETTY_FUNCTION__ << ": " << this->shared_count << " (Weak=++" << this->weak_count << ")" << std::endl;
#endif
                ++this->weak_count;
                assert(pthread_mutex_unlock(&this->mutex) == 0);
            }

            void weak_release() // throw()
            {
                assert(pthread_mutex_lock(&this->mutex) == 0);
#ifdef OUTPUT_REF_COUNTED
                OUTPUT_REF_COUNTED << static_cast<const void*>(this) << ": " << __PRETTY_FUNCTION__ << ": " << this->shared_count << " (Weak=--" << this->weak_count << ")" << std::endl;
#endif
                bool release_this = --this->weak_count == 0;
                assert(pthread_mutex_unlock(&this->mutex) == 0);

                if (release_this)
                {
                    this->destroy();
                }
            }

            long use_count() const // throw()
            {
                assert(pthread_mutex_lock(&this->mutex) == 0);
#ifdef OUTPUT_REF_COUNTED
                OUTPUT_REF_COUNTED << static_cast<const void*>(this) << ": " << __PRETTY_FUNCTION__ << ": " << this->shared_count << " (Weak=" << this->weak_count << ")" << std::endl;
#endif
                count_type value = this->shared_count;
                assert(pthread_mutex_unlock(&this->mutex) == 0);

                return value;
            }
        };
    }
}
