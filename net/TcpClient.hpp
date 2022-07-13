//
// Created by liupengcheng on 2021/7/21.
//

#ifndef LIBPCEV_TCPCLIENT_HPP
#define LIBPCEV_TCPCLIENT_HPP

#include <libpcev/base/noncopyable.hpp>
#include <libpcev/event/header.hpp>
#include <libpcev/net/InetAddress.hpp>
#include <libpcev/net/Socket.hpp>
#include <libpcev/net/Connector.hpp>

#include <string>

namespace pcev
{
namespace net
{

    enum TcpClientRC {
        TCP_CLIENT_OK,
        TCP_CLIENT_CONNECT_ERR,
    };

    class TcpClient : pcev::noncopyable
    {
    public:
        TcpClient() = delete;

        TcpClient(pcev::event::EventLoop *loop,
                  const InetAddress &client_addr,
                  std::string client_name);

        ~TcpClient() = default;

        void set_connect_callback(const pcev::event::ConnectCallback &connect_cb) {
            connect_cb_ = connect_cb;
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

        void send(const std::string &msg);

    private:
        pcev::event::EventLoop     *loop_;
        std::shared_ptr<Socket>     client_sock_;
        std::shared_ptr<Connector>  connector_;
        InetAddress                 client_addr_;
        std::string                 client_name_;

        pcev::event::ConnectCallback       connect_cb_;
        pcev::event::MessageCallback       message_cb_;
        pcev::event::WriteCompleteCallback write_complete_cb_;
        pcev::event::CloseCallback         close_cb_;
        pcev::event::ErrorCallback         error_cb_;
    };

} // namespace net
} // namespace pcev

#endif //LIBPCEV_TCPCLIENT_HPP
