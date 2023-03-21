/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "_ptr_element.hpp"
#include "_ref_counted.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>

namespace ft
{
    template <typename T>
    class enable_shared_from_this;

    template <typename T>
    class weak_ptr;

    template <typename T>
    class shared_ptr
    {
    public:
        typedef typename _internal::element_type<T>::type element_type;
        typedef _internal::_shared_count counted_type;

    private:
        template <typename U>
        friend class shared_ptr;
        template <typename U>
        friend class weak_ptr;
        template <typename U>
        friend class enable_shared_from_this;

    private:
        template <typename TType, typename U, typename V>
        static inline typename _internal::enable_if<!_internal::is_array<TType>::value, void>::type _ptr_enable_shared_from_this(const ft::shared_ptr<T>* this_ptr, const U* p, const ft::enable_shared_from_this<V>* from)
        {
            if (from != NULL)
            {
                from->change_ownership(this_ptr, const_cast<U*>(p));
            }
        }

        template <typename>
        static inline void _ptr_enable_shared_from_this(...)
        {
        }

    private:
        element_type* ptr;
        counted_type ref;

    public:
        shared_ptr() throw()
            : ptr(NULL), ref() {}

        template <typename U>
        explicit shared_ptr(U* p)
            : ptr(p), ref()
        {
            _internal::assert_convertible<U, element_type>();
            counted_type(p).swap(this->ref);
            _ptr_enable_shared_from_this<T>(this, p, p);
        }

        template <typename U, typename TDelete>
        shared_ptr(U* p, TDelete del)
            : ptr(p), ref(p, del)
        {
            _internal::assert_convertible<U, element_type>();
            _ptr_enable_shared_from_this<T>(this, p, p);
        }

        template <typename U, typename TDelete, typename TAlloc>
        shared_ptr(U* p, TDelete del, TAlloc alloc)
            : ptr(p), ref(p, del, alloc)
        {
            _internal::assert_convertible<U, element_type>();
            _ptr_enable_shared_from_this<T>(this, p, p);
        }

        shared_ptr(const shared_ptr& that) throw()
            : ptr(that.ptr), ref(that.ref) {}

        template <typename U>
        shared_ptr(const shared_ptr<U>& that) throw()
            : ptr(that.ptr), ref(that.ref)
        {
            _internal::assert_convertible<U, T>();
        }

        template <typename U>
        shared_ptr(const shared_ptr<U>& that, element_type* p) throw()
            : ptr(p), ref(that.ref) {}

        template <typename U>
        explicit shared_ptr(const weak_ptr<U>& that)
            : ref(that.ref)
        {
            _internal::assert_convertible<U, T>();

            this->ptr = that.ptr;
        }

        // Internal BEGIN
        template <typename TStorage, typename TInitializer>
        shared_ptr(_internal::internal_tag, const TStorage& storage, TInitializer init)
            : ptr(), ref(_internal::internal_tag(), &this->ptr, storage, init)
        {
            _ptr_enable_shared_from_this<T>(this, this->ptr, this->ptr);
        }
        // Internal END

        ~shared_ptr() throw() {}

        shared_ptr& operator=(const shared_ptr& that) throw()
        {
            shared_ptr(that).swap(*this);
            return *this;
        }

        template <typename U>
        shared_ptr& operator=(const shared_ptr<U>& that) throw()
        {
            _internal::assert_convertible<U, T>();

            shared_ptr(that).swap(*this);
            return *this;
        }

        void reset() throw()
        {
            shared_ptr().swap(*this);
        }

        template <typename U>
        void reset(U* p)
        {
            assert(p == NULL || p != this->ptr);

            shared_ptr(p).swap(*this);
        }

        template <typename U>
        void reset(const shared_ptr<U>& that, element_type* p) throw()
        {
            shared_ptr(that, p).swap(*this);
        }

        typename _internal::dereference<T>::type operator*() const throw()
        {
            assert(this->ptr != NULL);

            return *this->ptr;
        }

        typename _internal::member_access<T>::type operator->() const throw()
        {
            assert(this->ptr != NULL);

            return this->ptr;
        }

        typename _internal::array_access<T>::type operator[](std::ptrdiff_t i) const throw()
        {
            assert(this->ptr != NULL);
            assert(static_cast<std::size_t>(i) < _internal::array_extent<T>::value || _internal::array_extent<T>::value == 0);

            return this->ptr[i];
        }

        element_type* get() const throw()
        {
            return this->ptr;
        }

        bool unique() const throw()
        {
            return this->ref.unique();
        }

        long use_count() const throw()
        {
            return this->ref.use_count();
        }

        // explicit operator bool
        void unspecified_bool_type_func() const {}
        typedef void (shared_ptr::*unspecified_bool_type)() const;
        operator unspecified_bool_type() const throw()
        {
            return !this->ptr ? NULL : &shared_ptr::unspecified_bool_type_func;
        }

        void swap(shared_ptr& that) throw()
        {
            std::swap(this->ptr, that.ptr);
            this->ref.swap(that.ref);
        }
    };

    template <typename T, typename U>
    bool operator==(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) throw()
    {
        return lhs.get() == rhs.get();
    }

    template <typename T, typename U>
    bool operator!=(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) throw()
    {
        return lhs.get() != rhs.get();
    }

    template <typename T, typename U>
    bool operator<(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) throw()
    {
        return lhs.get() < rhs.get();
    }

    template <typename T, typename U>
    bool operator<=(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) throw()
    {
        return lhs.get() <= rhs.get();
    }

    template <typename T, typename U>
    bool operator>(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) throw()
    {
        return lhs.get() > rhs.get();
    }

    template <typename T, typename U>
    bool operator>=(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) throw()
    {
        return lhs.get() >= rhs.get();
    }

    template <typename T>
    void swap(shared_ptr<T>& lhs, shared_ptr<T>& rhs) throw()
    {
        lhs.swap(rhs);
    }

    template <typename T>
    typename shared_ptr<T>::element_type* get_pointer(const shared_ptr<T>& p) throw()
    {
        return p.get();
    }

    template <typename T, typename TSource>
    shared_ptr<T> static_pointer_cast(const shared_ptr<TSource>& that) throw()
    {
        // Compile-time test
        static_cast<void>(static_cast<T*>(static_cast<TSource*>(NULL)));

        return shared_ptr<T>(that, static_cast<typename shared_ptr<T>::element_type*>(that.get()));
    }

    template <typename T, typename TSource>
    shared_ptr<T> const_pointer_cast(const shared_ptr<TSource>& that) throw()
    {
        // Compile-time test
        static_cast<void>(const_cast<T*>(static_cast<TSource*>(NULL)));

        return shared_ptr<T>(that, const_cast<typename shared_ptr<T>::element_type*>(that.get()));
    }

    template <typename T, typename TSource>
    shared_ptr<T> dynamic_pointer_cast(const shared_ptr<TSource>& that) throw()
    {
        // Compile-time test
        static_cast<void>(dynamic_cast<T*>(static_cast<TSource*>(NULL)));

        // Run-time test
        typename shared_ptr<T>::element_type* p = dynamic_cast<typename shared_ptr<T>::element_type*>(that.get());
        if (!p)
        {
            return shared_ptr<T>();
        }

        return shared_ptr<T>(that, p);
    }

    template <typename T, typename TSource>
    shared_ptr<T> reinterpret_pointer_cast(const shared_ptr<TSource>& that) throw()
    {
        // Compile-time test
        static_cast<void>(reinterpret_cast<T*>(static_cast<TSource*>(NULL)));

        return shared_ptr<T>(that, reinterpret_cast<typename shared_ptr<T>::element_type*>(that.get()));
    }
}
