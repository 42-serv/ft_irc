/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "bootstrap.hpp"

#include "event_worker_group.hpp"
#include "logger.hpp"
#include "serv_types.hpp"
#include "server_channel.hpp"
#include "socket_utils.hpp"

#include <smart_ptr/shared_ptr.hpp>

#include <exception>
#include <string>

ft::serv::bootstrap::bootstrap(const ft::shared_ptr<event_worker_group>& boss_group, const ft::shared_ptr<event_worker_group>& child_group, make_server_t make_server, make_client_t make_client)
    : boss_group(boss_group),
      child_group(child_group),
      make_server(make_server),
      make_client(make_client),
      success(),
      ended()
{
    boss_group->wait_all();
    child_group->wait_all();
}

ft::serv::bootstrap::~bootstrap()
{
    this->finalize();
}

static const char* _to_c_str_nullable(const std::string& str)
{
    return str.empty() ? null : str.c_str();
}

bool ft::serv::bootstrap::start_server(const std::string& host_str, const std::string& serv_str, void* arg)
{
    assert(this->make_server);

    const ident_t boss_ident = socket_utils::bind_socket(_to_c_str_nullable(host_str), _to_c_str_nullable(serv_str));
    if (boss_ident < 0)
    {
        return false;
    }

    try
    {
        socket_utils::set_nonblocking(boss_ident, true);
        socket_utils::listen_socket(boss_ident);
    }
    catch (const std::exception& e)
    {
        socket_utils::close_socket(boss_ident);
        throw;
    }

    std::string host;
    int serv;
    socket_utils::name_socket(boss_ident, host, serv);
    const ft::shared_ptr<event_channel_base> boss = (*this->make_server)(boss_ident, host, serv, this->child_group, arg);
    logger::trace("Boot Server (%d, %s, %d)", boss_ident, host.c_str(), serv);
    boss->set_loop(this->boss_group->next());
    boss->loop_register();

    return true;
}

bool ft::serv::bootstrap::start_client(const std::string& host_str, const std::string& serv_str, void* arg)
{
    assert(this->make_client);

    const ident_t child_ident = ft::serv::socket_utils::connect_socket(_to_c_str_nullable(host_str), _to_c_str_nullable(serv_str));
    if (child_ident < 0)
    {
        return false;
    }

    try
    {
        socket_utils::set_nonblocking(child_ident, true);
    }
    catch (const std::exception& e)
    {
        socket_utils::close_socket(child_ident);
        throw;
    }

    std::string host;
    int serv;
    socket_utils::name_socket(child_ident, host, serv);
    const ft::shared_ptr<event_channel_base> child = (*this->make_client)(child_ident, host, serv, arg);
    logger::trace("Boot Client (%d, %s, %d)", child_ident, host.c_str(), serv);
    child->set_loop(this->child_group->next());
    child->loop_register();

    return true;
}

void ft::serv::bootstrap::set_success()
{
    this->success = true;
}

void ft::serv::bootstrap::finalize()
{
    if (!this->ended)
    {
        this->ended = true;
        if (!this->success)
        {
            this->boss_group->shutdown_all();
            if (this->boss_group != this->child_group)
            {
                this->child_group->shutdown_all();
            }
        }
        this->boss_group->join_all();
        if (this->boss_group != this->child_group)
        {
            this->child_group->join_all();
        }
    }
}
