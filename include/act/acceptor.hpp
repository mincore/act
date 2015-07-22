/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef ACT_ACCEPTOR_HPP_INCLUDED
#define ACT_ACCEPTOR_HPP_INCLUDED

#include <act/awaiter.hpp>

namespace act { namespace detail
{
    template<class Acceptor>
    struct accept_awaiter
    {
        using socket = typename Acceptor::protocol_type::socket;

        Acceptor& _acceptor;
        socket _sock;
        error_code _ec;

        accept_awaiter(Acceptor& acceptor)
          : _acceptor(acceptor)
          , _sock(_acceptor.get_io_service())
        {}

        bool await_ready() const
        {
            return false;
        }

        template<class F>
        void await_suspend(F&& cb)
        {
            _acceptor.async_accept(_sock, [&_ec=_ec, cb=mv(cb)](error_code ec) mutable
            {
                _ec = ec;
                cb();
            });
        }

        socket await_resume()
        {
            error_handler<true>::report(_ec);
            return std::move(_sock);
        }
    };
}}

namespace act
{
    template<class Acceptor>
    inline detail::accept_awaiter<Acceptor> accept(Acceptor& acceptor)
    {
        return {acceptor};
    }

    template<class Acceptor, class Socket>
    inline auto accept(Acceptor& acceptor, Socket& socket)
    {
        return make_awaiter<void>([&](auto&& cb)
        {
            acceptor.async_accept(socket, std::move(cb));
        });
    }

    template<class Acceptor, class Socket>
    inline auto accept(Acceptor& acceptor, Socket& socket, error_code& ec)
    {
        return make_awaiter<void>([&](auto&& cb)
        {
            acceptor.async_accept(socket, std::move(cb));
        }, ec);
    }

    template<class Acceptor, class Socket>
    inline auto accept(Acceptor& acceptor, Socket& socket, typename Acceptor::endpoint_type& endpoint)
    {
        return make_awaiter<void>([&](auto&& cb)
        {
            acceptor.async_accept(socket, endpoint, std::move(cb));
        });
    }

    template<class Acceptor, class Socket>
    inline auto accept(Acceptor& acceptor, Socket& socket, typename Acceptor::endpoint_type& endpoint, error_code& ec)
    {
        return make_awaiter<void>([&](auto&& cb)
        {
            acceptor.async_accept(socket, endpoint, std::move(cb));
        }, ec);
    }
}

#endif