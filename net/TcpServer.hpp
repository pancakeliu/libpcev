//
// Created by liupengcheng on 2021/6/29.
//

#ifndef LIBPCEV_TCPSERVER_HPP
#define LIBPCEV_TCPSERVER_HPP

#include <libpcev/base/noncopyable.hpp>
#include <libpcev/event/header.hpp>
#include <libpcev/event/Callbacks.hpp>
#include <libpcev/net/InetAddress.hpp>
#include <libpcev/net/Socket.hpp>
#include <libpcev/net/Accepter.hpp>

#include <string>

namespace pcev
{
namespace net
{
    enum TcpServerRC {
        TCP_SERVER_OK,
        TCP_SERVER_LISTEN_ERR,
        TCP_SERVER_ACCEPT_ERR,
    };

    class TcpServer : pcev::noncopyable
    {
    public:
        TcpServer() = delete;

        TcpServer(pcev::event::EventLoop *loop,
                  const InetAddress &listen_addr,
                  std::string server_name,
                  SocketOptions option = SocketOptions::K_SOCKET_OPTION_REUSEPORT);

        ~TcpServer() = default;

        void set_accept_callback(const pcev::event::AcceptCallback &accpet_cb) {
            accept_cb_ = accpet_cb;
        }
        void set_message_callback(const pcev::event::MessageCallback &message_cb) {
            message_cb_ = message_cb;
        }
        void set_write_complete_callback(const pcev::event::WriteCompleteCallback &write_complete_cb) {
            write_complete_cb_ = write_complete_cb;
        }
        void set_close_callback(const pcev::event::CloseCallback &close_cb) {
            close_cb_ = close_cb;
        }
        void set_error_callback(const pcev::event::ErrorCallback &error_cb) {
            error_cb_ = error_cb;
        }

        int start();

    private:
        pcev::event::EventLoop    *loop_;
        std::shared_ptr<Socket>    server_sock_;
        std::shared_ptr<Accepter>  accepter_;
        InetAddress                listen_addr_;
        std::string                server_name_;
        SocketOptions              option_;

        pcev::event::AcceptCallback        accept_cb_;
        pcev::event::MessageCallback       message_cb_;
        pcev::event::WriteCompleteCallback write_complete_cb_;
        pcev::event::CloseCallback         close_cb_;
        pcev::event::ErrorCallback         error_cb_;
    };

} // namespace net
} // namespace pcev

#endif //LIBPCEV_TCPSERVER_HPP
