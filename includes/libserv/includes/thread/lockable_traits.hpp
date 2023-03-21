/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#define FT_DEFINE_HAS_MEMBER(member_name)                                 \
    template <typename T>                                                 \
    struct has_member_##member_name                                       \
    {                                                                     \
        typedef char (&yes)[1];                                           \
        typedef char (&no)[2];                                            \
                                                                          \
        struct fallback_t                                                 \
        {                                                                 \
            int member_name;                                              \
        };                                                                \
        struct derived_t : T, fallback_t                                  \
        {                                                                 \
            derived_t();                                                  \
        };                                                                \
                                                                          \
        template <int fallback_t::*>                                      \
        struct t;                                                         \
                                                                          \
        template <typename U>                                             \
        static no f(t<&U::member_name>*);                                 \
        template <typename U>                                             \
        static yes f(...);                                                \
                                                                          \
        static const bool value = sizeof(f<derived_t>(0)) == sizeof(yes); \
    }

namespace ft
{
    namespace _internal
    {
        FT_DEFINE_HAS_MEMBER(lock);
        FT_DEFINE_HAS_MEMBER(unlock);
        FT_DEFINE_HAS_MEMBER(try_lock);
    }

    template <typename T>
    struct is_lockable
    {
        static const bool value = _internal::has_member_lock<T>::value &&
                                  _internal::has_member_unlock<T>::value;
    };

    template <typename T>
    struct is_try_lockable
    {
        static const bool value = is_lockable<T>::value &&
                                  _internal::has_member_try_lock<T>::value;
    };
}
