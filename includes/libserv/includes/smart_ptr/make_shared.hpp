/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "shared_ptr.hpp"

#include <cstddef>

namespace ft
{
    namespace _internal
    {
        // single
        template <typename T, typename TAlloc>
        struct deleter_storage
        {
            typedef TAlloc allocate_type;

            unsigned char data[sizeof(T)];
            TAlloc alloc;
            bool init;

        public:
            deleter_storage(const TAlloc& alloc) throw() : alloc(alloc), init(false) {}
            deleter_storage(const deleter_storage& that) throw() : alloc(that.alloc), init(that.init) {}
            ~deleter_storage() {}

        public:
            template <typename U>
            void operator()(U* p) const throw()
            {
                if (!this->init)
                {
                    return;
                }

                static_cast<T*>(p)->~T();
            }

        public:
            T* get_data() throw() { return reinterpret_cast<T*>(this->data); }
            T* get_dynamic() const throw() { return NULL; }
            const allocate_type& get_allocator() const throw() { return this->alloc; }
            std::size_t size() const throw() { return 1; }
            void set() throw() { this->init = true; }

        private:
            deleter_storage& operator=(const deleter_storage&);
        };

        // bounded array
        template <typename T, std::size_t N, typename TAlloc>
        struct deleter_storage<T[N], TAlloc>
        {
            typedef TAlloc allocate_type;
            typedef typename _internal::scalar_type<T>::type single_type;
            static const std::size_t single_count = _internal::scalar_count<T>::value;

            unsigned char data[N * sizeof(T)];
            TAlloc alloc;
            bool init;

        public:
            deleter_storage(const TAlloc& alloc) throw() : alloc(alloc), init(false) {}
            deleter_storage(const deleter_storage& that) throw() : alloc(that.alloc), init(that.init) {}
            ~deleter_storage() {}

        public:
            template <typename U>
            void operator()(U* p) const throw()
            {
                if (!this->init)
                {
                    return;
                }

                single_type* const arr = reinterpret_cast<single_type*>(p);
                const std::size_t n = this->size();
                for (std::size_t i = 0; i < n; i++)
                {
                    arr[i].~single_type();
                }
            }

        public:
            T* get_data() throw() { return reinterpret_cast<T*>(this->data); }
            T* get_dynamic() const throw() { return NULL; }
            const allocate_type& get_allocator() const throw() { return this->alloc; }
            std::size_t size() const throw() { return N * single_count; }
            void set() throw() { this->init = true; }

        private:
            deleter_storage& operator=(const deleter_storage&);
        };

        // unbounded array
        template <typename T, typename TAlloc>
        struct deleter_storage<T[], TAlloc>
        {
            typedef TAlloc allocate_type;
            typedef typename _internal::scalar_type<T>::type single_type;
            static const std::size_t single_count = _internal::scalar_count<T>::value;

            T* data;
            TAlloc alloc;
            std::size_t n;
            bool init;

        public:
            deleter_storage(const TAlloc& alloc, T* data, std::size_t n) throw() : alloc(alloc), data(data), n(n), init(false) {}
            deleter_storage(const deleter_storage& that) throw() : alloc(that.alloc), data(that.data), n(that.n), init(that.init) {}

            template <typename U>
            void operator()(U* p) const throw()
            {
                if (!this->init)
                {
                    return;
                }

                typedef typename TAlloc::template rebind<T>::other alloc_type;

                alloc_type alloc(this->alloc);

                single_type* const arr = reinterpret_cast<single_type*>(p);
                const std::size_t n = this->size();
                for (std::size_t i = 0; i < n; i++)
                {
                    arr[i].~single_type();
                }

                alloc.deallocate(static_cast<T*>(p), this->n);
            }

        public:
            T* get_data() throw() { return this->data; }
            T* get_dynamic() const throw() { return this->data; }
            const allocate_type& get_allocator() const throw() { return this->alloc; }
            std::size_t size() const throw() { return this->n * single_count; }
            void set() throw() { this->init = true; }

        private:
            deleter_storage& operator=(const deleter_storage&);
        };

        // single init
        template <typename T>
        struct single_initializer_0
        {
        public:
            single_initializer_0() {}
            single_initializer_0(const single_initializer_0&) {}
            ~single_initializer_0() {}

        public:
            template <typename TStorage>
            void operator()(TStorage& storage) const
            {
                ::new (storage.get_data()) T;
            }

        private:
            single_initializer_0& operator=(const single_initializer_0&);
        };

        template <typename T, typename A1>
        struct single_initializer_1
        {
            const A1& a1;

        public:
            single_initializer_1(const A1& a1) : a1(a1) {}
            single_initializer_1(const single_initializer_1& that) : a1(that.a1) {}
            ~single_initializer_1() {}

        public:
            template <typename TStorage>
            void operator()(TStorage& storage) const
            {
                ::new (storage.get_data()) T(const_cast<A1&>(this->a1));
            }

        private:
            single_initializer_1& operator=(const single_initializer_1&);
        };

        template <typename T, typename A1, typename A2>
        struct single_initializer_2
        {
            const A1& a1;
            const A2& a2;

        public:
            single_initializer_2(const A1& a1, const A2& a2) : a1(a1), a2(a2) {}
            single_initializer_2(const single_initializer_2& that) : a1(that.a1), a2(that.a2) {}
            ~single_initializer_2() {}

        public:
            template <typename TStorage>
            void operator()(TStorage& storage) const
            {
                ::new (storage.get_data()) T(const_cast<A1&>(this->a1), const_cast<A2&>(this->a2));
            }

        private:
            single_initializer_2& operator=(const single_initializer_2&);
        };

        template <typename T, typename A1, typename A2, typename A3>
        struct single_initializer_3
        {
            const A1& a1;
            const A2& a2;
            const A3& a3;

        public:
            single_initializer_3(const A1& a1, const A2& a2, const A3& a3) : a1(a1), a2(a2), a3(a3) {}
            single_initializer_3(const single_initializer_3& that) : a1(that.a1), a2(that.a2), a3(that.a3) {}
            ~single_initializer_3() {}

        public:
            template <typename TStorage>
            void operator()(TStorage& storage) const
            {
                ::new (storage.get_data()) T(const_cast<A1&>(this->a1), const_cast<A2&>(this->a2), const_cast<A3&>(this->a3));
            }

        private:
            single_initializer_3& operator=(const single_initializer_3&);
        };

        template <typename T, typename A1, typename A2, typename A3, typename A4>
        struct single_initializer_4
        {
            const A1& a1;
            const A2& a2;
            const A3& a3;
            const A4& a4;

        public:
            single_initializer_4(const A1& a1, const A2& a2, const A3& a3, const A4& a4) : a1(a1), a2(a2), a3(a3), a4(a4) {}
            single_initializer_4(const single_initializer_4& that) : a1(that.a1), a2(that.a2), a3(that.a3), a4(that.a4) {}
            ~single_initializer_4() {}

        public:
            template <typename TStorage>
            void operator()(TStorage& storage) const
            {
                ::new (storage.get_data()) T(const_cast<A1&>(this->a1), const_cast<A2&>(this->a2), const_cast<A3&>(this->a3), const_cast<A4&>(this->a4));
            }

        private:
            single_initializer_4& operator=(const single_initializer_4&);
        };

        template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5>
        struct single_initializer_5
        {
            const A1& a1;
            const A2& a2;
            const A3& a3;
            const A4& a4;
            const A5& a5;

        public:
            single_initializer_5(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5) : a1(a1), a2(a2), a3(a3), a4(a4), a5(a5) {}
            single_initializer_5(const single_initializer_5& that) : a1(that.a1), a2(that.a2), a3(that.a3), a4(that.a4), a5(that.a5) {}
            ~single_initializer_5() {}

        public:
            template <typename TStorage>
            void operator()(TStorage& storage) const
            {
                ::new (storage.get_data()) T(const_cast<A1&>(this->a1), const_cast<A2&>(this->a2), const_cast<A3&>(this->a3), const_cast<A4&>(this->a4), const_cast<A5&>(this->a5));
            }

        private:
            single_initializer_5& operator=(const single_initializer_5&);
        };

        template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        struct single_initializer_6
        {
            const A1& a1;
            const A2& a2;
            const A3& a3;
            const A4& a4;
            const A5& a5;
            const A6& a6;

        public:
            single_initializer_6(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6) : a1(a1), a2(a2), a3(a3), a4(a4), a5(a5), a6(a6) {}
            single_initializer_6(const single_initializer_6& that) : a1(that.a1), a2(that.a2), a3(that.a3), a4(that.a4), a5(that.a5), a6(that.a6) {}
            ~single_initializer_6() {}

        public:
            template <typename TStorage>
            void operator()(TStorage& storage) const
            {
                ::new (storage.get_data()) T(const_cast<A1&>(this->a1), const_cast<A2&>(this->a2), const_cast<A3&>(this->a3), const_cast<A4&>(this->a4), const_cast<A5&>(this->a5), const_cast<A6&>(this->a6));
            }

        private:
            single_initializer_6& operator=(const single_initializer_6&);
        };

        template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
        struct single_initializer_7
        {
            const A1& a1;
            const A2& a2;
            const A3& a3;
            const A4& a4;
            const A5& a5;
            const A6& a6;
            const A7& a7;

        public:
            single_initializer_7(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7) : a1(a1), a2(a2), a3(a3), a4(a4), a5(a5), a6(a6), a7(a7) {}
            single_initializer_7(const single_initializer_7& that) : a1(that.a1), a2(that.a2), a3(that.a3), a4(that.a4), a5(that.a5), a6(that.a6), a7(that.a7) {}
            ~single_initializer_7() {}

        public:
            template <typename TStorage>
            void operator()(TStorage& storage) const
            {
                ::new (storage.get_data()) T(const_cast<A1&>(this->a1), const_cast<A2&>(this->a2), const_cast<A3&>(this->a3), const_cast<A4&>(this->a4), const_cast<A5&>(this->a5), const_cast<A6&>(this->a6), const_cast<A7&>(this->a7));
            }

        private:
            single_initializer_7& operator=(const single_initializer_7&);
        };

        template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
        struct single_initializer_8
        {
            const A1& a1;
            const A2& a2;
            const A3& a3;
            const A4& a4;
            const A5& a5;
            const A6& a6;
            const A7& a7;
            const A8& a8;

        public:
            single_initializer_8(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8) : a1(a1), a2(a2), a3(a3), a4(a4), a5(a5), a6(a6), a7(a7), a8(a8) {}
            single_initializer_8(const single_initializer_8& that) : a1(that.a1), a2(that.a2), a3(that.a3), a4(that.a4), a5(that.a5), a6(that.a6), a7(that.a7), a8(that.a8) {}
            ~single_initializer_8() {}

        public:
            template <typename TStorage>
            void operator()(TStorage& storage) const
            {
                ::new (storage.get_data()) T(const_cast<A1&>(this->a1), const_cast<A2&>(this->a2), const_cast<A3&>(this->a3), const_cast<A4&>(this->a4), const_cast<A5&>(this->a5), const_cast<A6&>(this->a6), const_cast<A7&>(this->a7), const_cast<A8&>(this->a8));
            }

        private:
            single_initializer_8& operator=(const single_initializer_8&);
        };

        template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        struct single_initializer_9
        {
            const A1& a1;
            const A2& a2;
            const A3& a3;
            const A4& a4;
            const A5& a5;
            const A6& a6;
            const A7& a7;
            const A8& a8;
            const A9& a9;

        public:
            single_initializer_9(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9) : a1(a1), a2(a2), a3(a3), a4(a4), a5(a5), a6(a6), a7(a7), a8(a8), a9(a9) {}
            single_initializer_9(const single_initializer_9& that) : a1(that.a1), a2(that.a2), a3(that.a3), a4(that.a4), a5(that.a5), a6(that.a6), a7(that.a7), a8(that.a8), a9(that.a9) {}
            ~single_initializer_9() {}

        public:
            template <typename TStorage>
            void operator()(TStorage& storage) const
            {
                ::new (storage.get_data()) T(const_cast<A1&>(this->a1), const_cast<A2&>(this->a2), const_cast<A3&>(this->a3), const_cast<A4&>(this->a4), const_cast<A5&>(this->a5), const_cast<A6&>(this->a6), const_cast<A7&>(this->a7), const_cast<A8&>(this->a8), const_cast<A9&>(this->a9));
            }

        private:
            single_initializer_9& operator=(const single_initializer_9&);
        };

        // array init
        template <typename T>
        struct array_initializer_0
        {
        public:
            array_initializer_0() {}
            array_initializer_0(const array_initializer_0&) {}
            ~array_initializer_0() {}

        public:
            template <typename TStorage>
            void operator()(TStorage& storage) const
            {
                typedef typename TStorage::single_type single_type;

                std::size_t i = 0;
                const std::size_t n = storage.size();
                single_type* const arr = reinterpret_cast<single_type*>(storage.get_data());
                try
                {
                    for (; i < n; i++)
                    {
                        ::new (_internal::addressof(arr[i])) single_type;
                    }
                }
                catch (...)
                {
                    while (i != 0)
                    {
                        --i;
                        arr[i].~single_type();
                    }
                    throw;
                }
            }

        private:
            array_initializer_0& operator=(const array_initializer_0&);
        };

        template <typename T, typename A1>
        struct array_initializer_1
        {
            const A1& a1;

        public:
            array_initializer_1(const A1& a1) : a1(a1) {}
            array_initializer_1(const array_initializer_1& that) : a1(that.a1) {}
            ~array_initializer_1() {}

        public:
            template <typename TStorage>
            void operator()(TStorage& storage) const
            {
                typedef typename TStorage::single_type single_type;

                std::size_t i = 0;
                const std::size_t n = storage.size();
                single_type* const arr = reinterpret_cast<single_type*>(storage.get_data());
                try
                {
                    for (; i < n; i++)
                    {
                        ::new (_internal::addressof(arr[i])) single_type(this->a1);
                    }
                }
                catch (...)
                {
                    while (i != 0)
                    {
                        --i;
                        arr[i].~single_type();
                    }
                    throw;
                }
            }

        private:
            array_initializer_1& operator=(const array_initializer_1&);
        };
    }

    template <typename T, typename TAlloc>
    typename _internal::enable_if<!_internal::is_array<T>::value, ft::shared_ptr<T> >::type allocate_shared(const TAlloc& a)
    {
        return ft::shared_ptr<T>(_internal::internal_tag(), _internal::deleter_storage<T, TAlloc>(a), _internal::single_initializer_0<T>());
    }

    template <typename T, typename TAlloc, typename A1>
    typename _internal::enable_if<!_internal::is_array<T>::value, ft::shared_ptr<T> >::type allocate_shared(const TAlloc& a, const A1& a1)
    {
        return ft::shared_ptr<T>(_internal::internal_tag(), _internal::deleter_storage<T, TAlloc>(a), _internal::single_initializer_1<T, A1>(a1));
    }

    template <typename T, typename TAlloc, typename A1, typename A2>
    typename _internal::enable_if<!_internal::is_array<T>::value, ft::shared_ptr<T> >::type allocate_shared(const TAlloc& a, const A1& a1, const A2& a2)
    {
        return ft::shared_ptr<T>(_internal::internal_tag(), _internal::deleter_storage<T, TAlloc>(a), _internal::single_initializer_2<T, A1, A2>(a1, a2));
    }

    template <typename T, typename TAlloc, typename A1, typename A2, typename A3>
    typename _internal::enable_if<!_internal::is_array<T>::value, ft::shared_ptr<T> >::type allocate_shared(const TAlloc& a, const A1& a1, const A2& a2, const A3& a3)
    {
        return ft::shared_ptr<T>(_internal::internal_tag(), _internal::deleter_storage<T, TAlloc>(a), _internal::single_initializer_3<T, A1, A2, A3>(a1, a2, a3));
    }

    template <typename T, typename TAlloc, typename A1, typename A2, typename A3, typename A4>
    typename _internal::enable_if<!_internal::is_array<T>::value, ft::shared_ptr<T> >::type allocate_shared(const TAlloc& a, const A1& a1, const A2& a2, const A3& a3, const A4& a4)
    {
        return ft::shared_ptr<T>(_internal::internal_tag(), _internal::deleter_storage<T, TAlloc>(a), _internal::single_initializer_4<T, A1, A2, A3, A4>(a1, a2, a3, a4));
    }

    template <typename T, typename TAlloc, typename A1, typename A2, typename A3, typename A4, typename A5>
    typename _internal::enable_if<!_internal::is_array<T>::value, ft::shared_ptr<T> >::type allocate_shared(const TAlloc& a, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
    {
        return ft::shared_ptr<T>(_internal::internal_tag(), _internal::deleter_storage<T, TAlloc>(a), _internal::single_initializer_5<T, A1, A2, A3, A4, A5>(a1, a2, a3, a4, a5));
    }

    template <typename T, typename TAlloc, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    typename _internal::enable_if<!_internal::is_array<T>::value, ft::shared_ptr<T> >::type allocate_shared(const TAlloc& a, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)
    {
        return ft::shared_ptr<T>(_internal::internal_tag(), _internal::deleter_storage<T, TAlloc>(a), _internal::single_initializer_6<T, A1, A2, A3, A4, A5, A6>(a1, a2, a3, a4, a5, a6));
    }

    template <typename T, typename TAlloc, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
    typename _internal::enable_if<!_internal::is_array<T>::value, ft::shared_ptr<T> >::type allocate_shared(const TAlloc& a, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)
    {
        return ft::shared_ptr<T>(_internal::internal_tag(), _internal::deleter_storage<T, TAlloc>(a), _internal::single_initializer_7<T, A1, A2, A3, A4, A5, A6, A7>(a1, a2, a3, a4, a5, a6, a7));
    }

    template <typename T, typename TAlloc, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
    typename _internal::enable_if<!_internal::is_array<T>::value, ft::shared_ptr<T> >::type allocate_shared(const TAlloc& a, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)
    {
        return ft::shared_ptr<T>(_internal::internal_tag(), _internal::deleter_storage<T, TAlloc>(a), _internal::single_initializer_8<T, A1, A2, A3, A4, A5, A6, A7, A8>(a1, a2, a3, a4, a5, a6, a7, a8));
    }

    template <typename T, typename TAlloc, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
    typename _internal::enable_if<!_internal::is_array<T>::value, ft::shared_ptr<T> >::type allocate_shared(const TAlloc& a, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)
    {
        return ft::shared_ptr<T>(_internal::internal_tag(), _internal::deleter_storage<T, TAlloc>(a), _internal::single_initializer_9<T, A1, A2, A3, A4, A5, A6, A7, A8, A9>(a1, a2, a3, a4, a5, a6, a7, a8, a9));
    }

    template <typename T, typename TAlloc>
    typename _internal::enable_if<_internal::is_bounded_array<T>::value, ft::shared_ptr<T> >::type allocate_shared(const TAlloc& a)
    {
        return ft::shared_ptr<T>(_internal::internal_tag(), _internal::deleter_storage<T, TAlloc>(a), _internal::array_initializer_0<T>());
    }

    template <typename T, typename TAlloc>
    typename _internal::enable_if<_internal::is_bounded_array<T>::value, ft::shared_ptr<T> >::type allocate_shared(const TAlloc& a, const typename _internal::element_type<T>::type& def)
    {
        typedef typename _internal::element_type<T>::type elem_type;
        return ft::shared_ptr<T>(_internal::internal_tag(), _internal::deleter_storage<T, TAlloc>(a), _internal::array_initializer_1<T, elem_type>(def));
    }

    template <typename T, typename TAlloc>
    typename _internal::enable_if<_internal::is_unbounded_array<T>::value, ft::shared_ptr<T> >::type allocate_shared(const TAlloc& a, std::size_t n)
    {
        typedef typename _internal::element_type<T>::type elem_type;
        typedef typename TAlloc::template rebind<elem_type>::other alloc_type;
        _internal::allocate_guard<alloc_type> guard(a, n);
        ft::shared_ptr<T> result = ft::shared_ptr<T>(_internal::internal_tag(), _internal::deleter_storage<T, alloc_type>(a, guard.get(), n), _internal::array_initializer_0<T>());
        guard.reset();
        return result;
    }

    template <typename T, typename TAlloc>
    typename _internal::enable_if<_internal::is_unbounded_array<T>::value, ft::shared_ptr<T> >::type allocate_shared(const TAlloc& a, std::size_t n, const typename _internal::element_type<T>::type& def)
    {
        typedef typename _internal::element_type<T>::type elem_type;
        typedef typename TAlloc::template rebind<elem_type>::other alloc_type;
        _internal::allocate_guard<alloc_type> guard(a, n);
        ft::shared_ptr<T> result = ft::shared_ptr<T>(_internal::internal_tag(), _internal::deleter_storage<T, alloc_type>(a, guard.get(), n), _internal::array_initializer_1<T, elem_type>(def));
        guard.reset();
        return result;
    }

    template <typename T>
    ft::shared_ptr<T> make_shared()
    {
        return ft::allocate_shared<T>(std::allocator<T>());
    }

    template <typename T, typename A1>
    ft::shared_ptr<T> make_shared(const A1& a1)
    {
        return ft::allocate_shared<T>(std::allocator<T>(), a1);
    }

    template <typename T, typename A1, typename A2>
    ft::shared_ptr<T> make_shared(const A1& a1, const A2& a2)
    {
        return ft::allocate_shared<T>(std::allocator<T>(), a1, a2);
    }

    template <typename T, typename A1, typename A2, typename A3>
    ft::shared_ptr<T> make_shared(const A1& a1, const A2& a2, const A3& a3)
    {
        return ft::allocate_shared<T>(std::allocator<T>(), a1, a2, a3);
    }

    template <typename T, typename A1, typename A2, typename A3, typename A4>
    ft::shared_ptr<T> make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4)
    {
        return ft::allocate_shared<T>(std::allocator<T>(), a1, a2, a3, a4);
    }

    template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5>
    ft::shared_ptr<T> make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
    {
        return ft::allocate_shared<T>(std::allocator<T>(), a1, a2, a3, a4, a5);
    }

    template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    ft::shared_ptr<T> make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)
    {
        return ft::allocate_shared<T>(std::allocator<T>(), a1, a2, a3, a4, a5, a6);
    }

    template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
    ft::shared_ptr<T> make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)
    {
        return ft::allocate_shared<T>(std::allocator<T>(), a1, a2, a3, a4, a5, a6, a7);
    }

    template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
    ft::shared_ptr<T> make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)
    {
        return ft::allocate_shared<T>(std::allocator<T>(), a1, a2, a3, a4, a5, a6, a7, a8);
    }

    template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
    ft::shared_ptr<T> make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)
    {
        return ft::allocate_shared<T>(std::allocator<T>(), a1, a2, a3, a4, a5, a6, a7, a8, a9);
    }
}
