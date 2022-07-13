//
// Created by liupengcheng on 2021/7/5.
//

#ifndef LIBPCEV_SOCKET_HPP
#define LIBPCEV_SOCKET_HPP

#include <libpcev/base/noncopyable.hpp>
#include <libpcev/net/InetAddress.hpp>

namespace pcev
{
namespace net
{

    enum SocketOptions {
        K_SOCKET_OPTION_NOT_REUSEPORT,
        K_SOCKET_OPTION_REUSEPORT,
    };

    enum SocketRC : int {
        SOCKET_RC_OK,
        SOCKET_ERROR,
        SET_SOCK_OPT_ERROR,
        BIND_ERROR,
        LISTEN_ERROR,
        CONNECT_ERROR,
        CONNECT_EINPROGRESS,
    };

    class Socket : public noncopyable
    {
    public:
        enum SockType {
            K_NOT_SET,
            K_TCP_SERVER,
            K_TCP_CLIENT,
        };
    public:
        Socket(const InetAddress &addr);
        ~Socket();

        int sock_fd() const {
            return sock_fd_;
        }

        int tcp_server(SocketOptions opt = SocketOptions::K_SOCKET_OPTION_NOT_REUSEPORT);
        int tcp_client();

    private:
        void close_socket();

    private:
        InetAddress addr_;
        int         sock_fd_{};
        SockType    sock_type_;

    }; // class Socket

} // namespace net
} // namespace pcev

#endif //LIBPCEV_SOCKET_HPP
