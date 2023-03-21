/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <cstddef>

namespace ft
{
    namespace _internal
    {
        struct internal_tag
        {
        };

        template <typename T, T Value>
        struct integral_constant
        {
            typedef T value_type;
            typedef integral_constant<T, Value> type;
            static const T value = Value;

            operator T() const { return Value; }
        };

        typedef integral_constant<bool, true> true_type;
        typedef integral_constant<bool, false> false_type;

        template <bool B, typename T = void>
        struct enable_if
        {
            typedef T type;
        };

        template <typename T>
        struct enable_if<false, T>
        {
        };

        template <typename TSource, typename TDest>
        struct convertible
        {
            typedef char (&yes)[1];
            typedef char (&no)[2];

            static yes f(TDest*);
            static no f(...);

            static const bool value = sizeof((f)(static_cast<TSource*>(0))) == sizeof(yes);
        };

        template <typename TSource, typename TDest>
        struct convertible<TSource, TDest[]>
        {
            static const bool value = false;
        };

        template <typename TSource, typename TDest>
        struct convertible<TSource[], TDest[]>
        {
            static const bool value = convertible<TSource[1], TDest[1]>::value;
        };

        template <typename TSource, std::size_t N, typename TDest>
        struct convertible<TSource[N], TDest[]>
        {
            static const bool value = convertible<TSource[1], TDest[1]>::value;
        };

        ///

        template <typename TSource, typename TDest>
        inline void assert_convertible() throw()
        {
            static_cast<void>(sizeof(char[convertible<TSource, TDest>::value ? 1 : -1]));
        }

        template <typename T>
        struct element_type
        {
            typedef T type;
        };

        template <typename T>
        struct element_type<T[]>
        {
            typedef T type;
        };

        template <typename T, std::size_t N>
        struct element_type<T[N]>
        {
            typedef T type;
        };

        template <typename T>
        struct dereference
        {
            typedef T& type;
        };

        template <>
        struct dereference<void>
        {
            typedef void type;
        };

        template <>
        struct dereference<const void>
        {
            typedef void type;
        };

        template <>
        struct dereference<volatile void>
        {
            typedef void type;
        };

        template <>
        struct dereference<const volatile void>
        {
            typedef void type;
        };

        template <typename T>
        struct dereference<T[]>
        {
            typedef void type;
        };

        template <typename T, std::size_t N>
        struct dereference<T[N]>
        {
            typedef void type;
        };

        template <typename T>
        struct member_access
        {
            typedef T* type;
        };

        template <typename T>
        struct member_access<T[]>
        {
            typedef void type;
        };

        template <typename T, std::size_t N>
        struct member_access<T[N]>
        {
            typedef void type;
        };

        template <typename T>
        struct array_access
        {
            typedef void type;
        };

        template <typename T>
        struct array_access<T[]>
        {
            typedef T& type;
        };

        template <typename T, std::size_t N>
        struct array_access<T[N]>
        {
            typedef T& type;
        };

        template <typename T>
        struct array_extent
        {
            static const std::size_t value = 0;
        };

        template <typename T, std::size_t N>
        struct array_extent<T[N]>
        {
            static const std::size_t value = N;
        };

        template <typename T>
        struct is_unbounded_array : false_type
        {
        };

        template <typename T>
        struct is_unbounded_array<T[]> : true_type
        {
        };

        template <typename T>
        struct is_unbounded_array<const T[]> : true_type
        {
        };

        template <typename T>
        struct is_unbounded_array<volatile T[]> : true_type
        {
        };

        template <typename T>
        struct is_unbounded_array<const volatile T[]> : true_type
        {
        };

        template <typename T>
        struct is_bounded_array : false_type
        {
        };
        template <typename T, std::size_t N>
        struct is_bounded_array<T[N]> : true_type
        {
        };

        template <typename T, std::size_t N>
        struct is_bounded_array<const T[N]> : true_type
        {
        };

        template <typename T, std::size_t N>
        struct is_bounded_array<volatile T[N]> : true_type
        {
        };

        template <typename T, std::size_t N>
        struct is_bounded_array<const volatile T[N]> : true_type
        {
        };

        template <typename T>
        struct is_array
            : integral_constant<
                  bool,
                  is_unbounded_array<T>::value ||
                      is_bounded_array<T>::value>
        {
        };

        template <typename T>
        inline T* addressof(T& arg) throw()
        {
            return reinterpret_cast<T*>(
                &const_cast<char&>(
                    reinterpret_cast<const volatile char&>(arg)));
        }

        template <typename T>
        struct scalar_type
        {
            typedef T type;
        };

        template <typename T, std::size_t N>
        struct scalar_type<T[N]>
        {
            typedef typename scalar_type<T>::type type;
        };

        template <typename T>
        struct scalar_count
        {
            static const std::size_t value = 1;
        };

        template <typename T, std::size_t N>
        struct scalar_count<T[N]>
        {
            static const std::size_t value = N * scalar_count<T>::value;
        };

        template <typename TAlloc>
        struct allocate_guard
        {
            typedef typename TAlloc::value_type value_type;
            typedef value_type* pointer;

            TAlloc alloc;
            std::size_t n;
            pointer ptr;

        public:
            allocate_guard(const TAlloc& alloc, std::size_t n = 1)
                : alloc(alloc), n(n)
            {
                this->ptr = this->alloc.allocate(this->n);
            }

            ~allocate_guard()
            {
                if (this->ptr != NULL)
                {
                    this->alloc.deallocate(this->ptr, this->n);
                }
            }

            pointer get() const throw() { return this->ptr; }
            void reset() throw() { this->ptr = NULL; }

        private:
            allocate_guard& operator=(const allocate_guard&);
        };
    }
}
