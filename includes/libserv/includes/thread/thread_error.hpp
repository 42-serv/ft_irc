/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

namespace ft
{
    class thread_error : public std::runtime_error
    {
    public:
        typedef int error_t;

    private:
        error_t e;

    public:
        explicit thread_error(error_t e) throw() : std::runtime_error(const_cast<const char*>(std::strerror(e))), e(e) {}
        thread_error(error_t e, const std::string& what_arg) throw() : std::runtime_error(what_arg), e(e) {}
        thread_error(error_t e, const char* what_arg) throw() : std::runtime_error(what_arg), e(e) {}

        error_t error() const throw() { return this->e; }
    };
}
