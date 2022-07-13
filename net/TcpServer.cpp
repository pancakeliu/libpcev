//
// Created by liupengcheng on 2021/6/29.
//

#include <libpcev/net/TcpServer.hpp>
#include <utility>

using namespace pcev::net;

TcpServer::TcpServer(pcev::event::EventLoop *loop, const InetAddress &listen_addr,
                     std::string server_name, SocketOptions option)
        : loop_(loop),
          listen_addr_(listen_addr),
          server_name_(std::move(server_name)),
          option_(option)
{
    assert(loop_);
}

int TcpServer::start() {
    server_sock_ = std::make_shared<Socket>(listen_addr_);
    int rc = server_sock_->tcp_server(option_);
    if (rc != SocketRC::SOCKET_RC_OK) {
        return TcpServerRC::TCP_SERVER_LISTEN_ERR;
    }
    accepter_ = std::make_shared<Accepter>(loop_, server_sock_->sock_fd());

    if (accept_cb_) {
        accepter_->set_accept_callback(accept_cb_);
    }
    if (message_cb_) {
        accepter_->set_message_callback(message_cb_);
    }
    if (write_complete_cb_) {
        accepter_->set_write_complete_callback(write_complete_cb_);
    }
    if (close_cb_) {
        accepter_->set_close_callback(close_cb_);
    }
    if (error_cb_) {
        accepter_->set_error_callback(error_cb_);
    }

    if (accepter_->accept() != 0) {
        return TcpServerRC::TCP_SERVER_ACCEPT_ERR;
    }

    return TcpServerRC::TCP_SERVER_OK;
}