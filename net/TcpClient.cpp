//
// Created by liupengcheng on 2021/7/21.
//

#include <libpcev/net/TcpClient.hpp>
#include <utility>

using namespace pcev::net;
using namespace pcev::event;

TcpClient::TcpClient(pcev::event::EventLoop *loop, const InetAddress &client_addr,
                     std::string client_name)
    : loop_(loop), client_addr_(client_addr), client_name_(std::move(client_name))
{
    assert(loop_);
}

int TcpClient::start() {
    client_sock_ = std::make_shared<Socket>(client_addr_);
    int rc = client_sock_->tcp_client();
    if (rc != SocketRC::SOCKET_RC_OK && rc != SocketRC::CONNECT_EINPROGRESS) {
        return TcpClientRC::TCP_CLIENT_CONNECT_ERR;
    }

    bool need_conn = false;
    if (rc == SocketRC::CONNECT_EINPROGRESS) {
        need_conn = true;
    }
    connector_ = std::make_shared<Connector>(loop_, client_sock_->sock_fd(), need_conn);
    if (connect_cb_) {
        connector_->set_connect_callback(connect_cb_);
    }
    if (message_cb_) {
        connector_->set_message_callback(message_cb_);
    }
    if (write_complete_cb_) {
        connector_->set_write_complete_callback(write_complete_cb_);
    }
    if (close_cb_) {
        connector_->set_close_callback(close_cb_);
    }
    if (error_cb_) {
        connector_->set_error_callback(error_cb_);
    }

    if (connector_->connect() != 0) {
        return TcpClientRC::TCP_CLIENT_CONNECT_ERR;
    }

    return TcpClientRC::TCP_CLIENT_OK;
}

void TcpClient::send(const std::string &msg) {
    connector_->send(msg);
}