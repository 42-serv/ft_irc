/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "__ref_counted_base_posix.hpp"
#include "bad_weak_ptr.hpp"

#include <stdexcept>

namespace ft
{
    namespace _internal
    {
        template <typename T>
        class _counted_impl : public _counted_base
        {
        private:
            T* ptr;

            _counted_impl(const _counted_impl&);
            _counted_impl& operator=(const _counted_impl&);

        public:
            explicit _counted_impl(T* ptr)
                : ptr(ptr) {}

            void dispose() throw()
            {
                ::delete ptr;
            }

            void destroy() throw()
            {
                ::delete this;
            }

        public:
            T* get_pointer() { return this->ptr; }
        };

        template <typename TPointer, typename TDelete>
        class _counted_impl_del : public _counted_base
        {
        private:
            TPointer ptr;
            TDelete del;

            _counted_impl_del(const _counted_impl_del&);
            _counted_impl_del& operator=(const _counted_impl_del&);

        public:
            explicit _counted_impl_del(TPointer ptr, const TDelete& del)
                : ptr(ptr), del(del) {}

            void dispose() throw()
            {
                this->del(ptr);
            }

            void destroy() throw()
            {
                ::delete this;
            }

        public:
            TPointer get_pointer() { return this->ptr; }

            TDelete& get_deleter() { return this->del; }
            const TDelete& get_deleter() const { return this->del; }
        };

        template <typename TPointer, typename TDelete, typename TAlloc>
        class _counted_impl_del_alloc : public _counted_base
        {
        private:
            TPointer ptr;
            TDelete del;
            TAlloc alloc;

            _counted_impl_del_alloc(const _counted_impl_del_alloc&);
            _counted_impl_del_alloc& operator=(const _counted_impl_del_alloc&);

        public:
            explicit _counted_impl_del_alloc(TPointer ptr, const TDelete& del, const TAlloc& alloc)
                : ptr(ptr), del(del), alloc(alloc) {}

            void dispose() throw()
            {
                this->del(ptr);
            }

            void destroy() throw()
            {
                typedef _counted_impl_del_alloc counted_type;
                typedef typename TAlloc::template rebind<counted_type>::other alloc_type;

                alloc_type alloc_counted(this->alloc);
                static_cast<counted_type*>(this)->~counted_type();
                alloc_counted.deallocate(this, 1);
            }

        public:
            TPointer get_pointer() { return this->ptr; }
            void init_pointer(TPointer ptr) { this->ptr = ptr; }

            TDelete& get_deleter() { return this->del; }
            const TDelete& get_deleter() const { return this->del; }

            TAlloc& get_allocator() { return this->alloc; }
            const TAlloc& get_allocator() const { return this->alloc; }
        };

        class _weak_count;

        class _shared_count
        {
        private:
            friend class _weak_count;

        private:
            _counted_base* ptr;

        public:
            _shared_count() throw()
                : ptr(NULL) {}

            explicit _shared_count(_counted_base* ptr) throw()
                : ptr(ptr) {}

            template <typename T>
            explicit _shared_count(T* p)
                : ptr(NULL)
            {
                try
                {
                    this->ptr = ::new _counted_impl<T>(p);
                }
                catch (...)
                {
                    ::delete p;
                    throw;
                }
            }

            template <typename TPointer, typename TDelete>
            _shared_count(TPointer p, TDelete del)
            {
                try
                {
                    this->ptr = ::new _counted_impl_del<TPointer, TDelete>(p, del);
                }
                catch (...)
                {
                    del(p);
                    throw;
                }
            }

            template <typename TPointer, typename TDelete, typename TAlloc>
            _shared_count(TPointer p, TDelete del, TAlloc alloc)
            {
                typedef _counted_impl_del_alloc<TPointer, TDelete, TAlloc> counted_type;
                typedef typename TAlloc::template rebind<counted_type>::other alloc_type;

                alloc_type alloc_counted(alloc);
                _internal::allocate_guard<alloc_type> guard(alloc_counted);

                counted_type* this_ptr = guard.get();

                try
                {
                    ::new (this_ptr) counted_type(p, del, alloc);
                }
                catch (...)
                {
                    del(p);
                    throw;
                }

                this->ptr = this_ptr;
                guard.reset();
            }

            // Internal BEGIN
            template <typename T, typename TStorage, typename TInitializer>
            _shared_count(_internal::internal_tag, T** pp, const TStorage& storage, TInitializer init)
            {
                typedef _counted_impl_del_alloc<T*, TStorage, typename TStorage::allocate_type> counted_type;
                typedef typename TStorage::allocate_type::template rebind<counted_type>::other alloc_type;

                alloc_type alloc_counted(storage.get_allocator());
                _internal::allocate_guard<alloc_type> guard(alloc_counted);

                counted_type* this_ptr = guard.get();

                ::new (this_ptr) counted_type(static_cast<T*>(NULL), storage, alloc_counted);

                TStorage& this_storage = this_ptr->get_deleter();
                T* this_p = storage.get_dynamic();
                if (this_p == NULL)
                {
                    // is not dynamic
                    this_p = this_storage.get_data();
                }
                this_ptr->init_pointer(this_p);

                init(this_storage);
                this_storage.set();

                this->ptr = this_ptr;
                *pp = this_p;
                guard.reset();
            }
            // Internal END

            _shared_count(const _shared_count& that) throw()
                : ptr(that.ptr)
            {
                if (this->ptr != NULL)
                {
                    this->ptr->add_ref_copy();
                }
            }

            explicit _shared_count(const _weak_count& that);

            ~_shared_count()
            {
                if (this->ptr != NULL)
                {
                    this->ptr->release();
                }
            }

            _shared_count& operator=(const _shared_count& that) throw()
            {
                if (this->ptr != that.ptr)
                {
                    _counted_base* tmp = that.ptr;
                    if (that.ptr != NULL)
                    {
                        that.ptr->add_ref_copy();
                    }
                    if (this->ptr != NULL)
                    {
                        this->ptr->release();
                    }
                    this->ptr = tmp;
                }
                return *this;
            }

            void swap(_shared_count& that) throw()
            {
                _counted_base* tmp = that.ptr;
                that.ptr = this->ptr;
                this->ptr = tmp;
            }

            long use_count() const throw()
            {
                if (this->ptr == NULL)
                {
                    return 0;
                }
                return this->ptr->use_count();
            }

            bool unique() const throw()
            {
                return this->use_count() == 1;
            }

            bool empty() const throw()
            {
                return this->ptr == NULL;
            }

            bool operator==(const _shared_count& that) const throw()
            {
                return this->ptr == that.ptr;
            }

            bool operator==(const _weak_count& that) const throw();
        };

        class _weak_count
        {
        private:
            friend class _shared_count;

        private:
            _counted_base* ptr;

        public:
            _weak_count() throw()
                : ptr(NULL) {}

            _weak_count(const _shared_count& that) throw()
                : ptr(that.ptr)
            {
                if (this->ptr != NULL)
                {
                    this->ptr->weak_add_ref();
                }
            }

            _weak_count(const _weak_count& that) throw()
                : ptr(that.ptr)
            {
                if (this->ptr != NULL)
                {
                    this->ptr->weak_add_ref();
                }
            }

            ~_weak_count()
            {
                if (this->ptr != NULL)
                {
                    this->ptr->weak_release();
                }
            }

            _weak_count& operator=(const _shared_count& that) throw()
            {
                if (this->ptr != that.ptr)
                {
                    _counted_base* tmp = that.ptr;
                    if (that.ptr != NULL)
                    {
                        that.ptr->weak_add_ref();
                    }
                    if (this->ptr != NULL)
                    {
                        this->ptr->weak_release();
                    }
                    this->ptr = tmp;
                }
                return *this;
            }

            _weak_count& operator=(const _weak_count& that) throw()
            {
                if (this->ptr != that.ptr)
                {
                    _counted_base* tmp = that.ptr;
                    if (that.ptr != NULL)
                    {
                        that.ptr->weak_add_ref();
                    }
                    if (this->ptr != NULL)
                    {
                        this->ptr->weak_release();
                    }
                    this->ptr = tmp;
                }
                return *this;
            }

            void swap(_weak_count& that) throw()
            {
                _counted_base* tmp = that.ptr;
                that.ptr = this->ptr;
                this->ptr = tmp;
            }

            long use_count() const throw()
            {
                if (this->ptr == NULL)
                {
                    return 0;
                }
                return this->ptr->use_count();
            }

            bool operator==(const _weak_count& that) const throw()
            {
                return this->ptr == that.ptr;
            }

            bool operator==(const _shared_count& that) const throw()
            {
                return this->ptr == that.ptr;
            }
        };

        inline _shared_count::_shared_count(const _weak_count& that)
            : ptr(that.ptr)
        {
            if (this->ptr == NULL || !this->ptr->add_ref_lock())
            {
                throw bad_weak_ptr();
            }
        }

        inline bool _shared_count::operator==(const _weak_count& that) const throw()
        {
            return this->ptr == that.ptr;
        }
    }
}
