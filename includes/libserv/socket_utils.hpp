/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "serv_exception.hpp"
#include "serv_types.hpp"

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstddef>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <string>

namespace ft
{
    namespace serv
    {
        namespace _internal
        {
            class ai_failed : public std::exception
            {
            private:
                error_t e;
                const char* s;

            public:
                ai_failed(error_t e) throw() : e(e), s(const_cast<const char*>(::gai_strerror(e))) {}
                ai_failed(error_t e, const char* s) throw() : e(e), s(s) {}
                virtual ~ai_failed() throw() {}
                virtual const char* what() const throw() { return this->s; }

                error_t error() const throw() { return this->e; }
            };

            struct gai_guard
            {
                struct ::addrinfo* result;

                gai_guard(const char* host, const char* serv, struct ::addrinfo& hints)
                {
                    const error_t gai_errno = ::getaddrinfo(host, serv, &hints, &this->result);
                    if (gai_errno != 0)
                    {
                        throw ai_failed(gai_errno);
                    }
                }

                ~gai_guard()
                {
                    ::freeaddrinfo(this->result);
                }

            private:
                gai_guard(const gai_guard&);
                gai_guard& operator=(const gai_guard&);
            };
        }

        struct socket_utils
        {
            static inline ident_t bind_socket(const char* const host, const char* const serv)
            {
                return socket_utils::forward_lookup(host, serv, AI_PASSIVE, &socket_utils::reuse_address_bind);
            }

            static inline ident_t connect_socket(const char* const host, const char* const serv)
            {
                return socket_utils::forward_lookup(host, serv, 0, &socket_utils::nosigpipe_connect);
            }

            static inline void listen_socket(const ident_t socket, const int backlog = FT_SERV_DEFAULT_LISTEN_BACKLOG)
            {
                if (::listen(socket, backlog) < 0)
                {
                    throw syscall_failed();
                }
            }

            static inline ident_t accept_socket(const ident_t socket, std::string& host, int& serv) throw()
            {
                sockaddr addr;
                socklen_t addr_len;

                ident_t child_socket;
                do
                {
                    addr_len = sizeof(addr);
                    child_socket = ::accept(socket, &addr, &addr_len);
                    if (child_socket < 0)
                    {
                        const syscall_failed e;
                        switch (e.error())
                        {
                        case EINTR:
                            continue;

                        default:
                            return -e.error();
                        }
                    }
                    break;
                } while (!0);

#ifndef __linux__
                {
                    const int value = true;
                    if (::setsockopt(child_socket, SOL_SOCKET, SO_NOSIGPIPE, &value, sizeof(value)) < 0)
                    {
                        ::close(child_socket);
                        return -1;
                    }
                }
#endif
                socket_utils::reverse_lookup(addr, addr_len, host, serv);
                return child_socket;
            }

            static inline void name_socket(const ident_t socket, std::string& host, int& serv)
            {
                sockaddr addr;
                socklen_t addr_len;

                addr_len = sizeof(addr);
                if (::getsockname(socket, &addr, &addr_len) < 0)
                {
                    throw syscall_failed();
                }
                socket_utils::reverse_lookup(addr, addr_len, host, serv);
            }

            static inline long recv_socket(const ident_t socket, void* const buf, const std::size_t len) throw()
            {
                staticassert(sizeof(long) >= sizeof(::ssize_t));

                ::ssize_t value;
                do
                {
                    value = ::recv(socket, buf, len, 0);
                    if (value < 0)
                    {
                        const syscall_failed e;
                        switch (e.error())
                        {
                        case EINTR:
                            continue;

                        default:
                            return -e.error();
                        }
                    }
                    break;
                } while (!0);

                return value;
            }

            static inline long send_socket(const ident_t socket, const void* const buf, const std::size_t len) throw()
            {
                staticassert(sizeof(long) >= sizeof(::ssize_t));

                ::ssize_t value;
                do
                {
#ifdef __linux__
                    value = ::send(socket, buf, len, MSG_NOSIGNAL);
#else
                    value = ::send(socket, buf, len, 0);
#endif
                    if (value < 0)
                    {
                        const syscall_failed e;
                        switch (e.error())
                        {
                        case EINTR:
                            continue;

                        default:
                            return -e.error();
                        }
                    }
                    break;
                } while (!0);

                return value;
            }

            static inline void finish_socket(const ident_t socket) throw()
            {
                if (::shutdown(socket, SHUT_WR) < 0)
                {
                    const syscall_failed e;
                    // ignore
                }
            }

            static inline void close_socket(const ident_t socket) throw()
            {
                if (::close(socket) < 0)
                {
                    const syscall_failed e;
                    // ignore
                }
            }

            static inline void set_socket_linger(const ident_t socket, const bool enable, const int time_in_seconds)
            {
                struct linger value;
                value.l_onoff = enable;
                value.l_linger = time_in_seconds;

                if (::setsockopt(socket, SOL_SOCKET, SO_LINGER, &value, sizeof(value)) < 0)
                {
                    throw syscall_failed();
                }
            }

            static inline void set_tcp_nodelay(const ident_t socket, const int value)
            {
                if (::setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value)) < 0)
                {
                    throw syscall_failed();
                }
            }

            static inline void set_nonblocking(const ident_t socket, const bool value)
            {
                int status_flags = ::fcntl(socket, F_GETFL, 0);
                if (status_flags < 0)
                {
                    throw syscall_failed();
                }

                if (value)
                {
                    status_flags |= O_NONBLOCK;
                }
                else
                {
                    status_flags &= ~O_NONBLOCK;
                }

                if (::fcntl(socket, F_SETFL, status_flags) < 0)
                {
                    throw syscall_failed();
                }
            }

        private:
            static inline int nosigpipe_connect(ident_t socket, const sockaddr* addr, socklen_t addr_len)
            {
#ifndef __linux__
                const int value = true;
                if (::setsockopt(socket, SOL_SOCKET, SO_NOSIGPIPE, &value, sizeof(value)) < 0)
                {
                    return -1;
                }
#endif
                return ::connect(socket, addr, addr_len);
            }

            static inline int reuse_address_bind(ident_t socket, const sockaddr* addr, socklen_t addr_len)
            {
                const int value = true;
                if (::setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)) < 0)
                {
                    return -1;
                }
                return ::bind(socket, addr, addr_len);
            }

            static inline ident_t forward_lookup(const char* const host, const char* const serv, int ai_hint_flags, int (*const func)(ident_t, const sockaddr*, socklen_t))
            {
                struct ::addrinfo hints;
                std::memset(&hints, 0, sizeof(hints));
                hints.ai_family = AF_UNSPEC;
                hints.ai_socktype = SOCK_STREAM;
                hints.ai_protocol = 0; // ANY

                // additional flags
                hints.ai_flags |= ai_hint_flags;

                try
                {
                    const _internal::gai_guard gai(host, serv, hints);

                    for (struct ::addrinfo* it = gai.result; it != null; it = it->ai_next)
                    {
                        ident_t socket = ::socket(it->ai_family, it->ai_socktype, it->ai_protocol);
                        if (socket < 0)
                        {
                            throw syscall_failed();
                        }

                        if ((*func)(socket, it->ai_addr, it->ai_addrlen) < 0)
                        {
                            ::close(socket);
                            continue;
                        }

                        return socket;
                    }

                    throw syscall_failed();
                }
                catch (const _internal::ai_failed& e)
                {
                    if (e.error() != EAI_NONAME && e.error() != EAI_SERVICE)
                    {
                        throw;
                    }
                }

                return -1;
            }

            static inline void reverse_lookup(const struct ::sockaddr& addr, const socklen_t addr_len, std::string& out_host, int& out_serv)
            {
                char host[NI_MAXHOST];
                char serv[NI_MAXSERV];
                if (::getnameinfo(&addr, addr_len, host, sizeof(host), serv, sizeof(serv), NI_NAMEREQD | NI_NUMERICSERV) != 0)
                {
                    if (::getnameinfo(&addr, addr_len, host, sizeof(host), serv, sizeof(serv), NI_NUMERICHOST | NI_NUMERICSERV) < 0)
                    {
                        throw syscall_failed();
                    }
                }

                out_host = host;
                std::istringstream iss(serv);
                iss >> out_serv;
            }
        };
    }
}
