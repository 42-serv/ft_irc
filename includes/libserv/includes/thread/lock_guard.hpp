/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

namespace ft
{
    template <typename T>
    class lock_guard
    {
    private:
        T& lock;

    public:
        lock_guard(T& lock) : lock(lock) { lock.lock(); }
        ~lock_guard() { lock.unlock(); }

    private:
        lock_guard(const lock_guard&);
        lock_guard& operator=(const lock_guard&);
    };
}
