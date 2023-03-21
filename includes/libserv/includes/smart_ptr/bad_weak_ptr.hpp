/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <exception>

namespace ft
{
    class bad_weak_ptr : public std::exception
    {
    public:
        bad_weak_ptr() throw() : std::exception() {}
        virtual ~bad_weak_ptr() throw() {}
        virtual const char* what() const throw() { return "bad weak pointer"; }
    };
}
