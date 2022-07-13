//
// Created by liupengcheng on 2021/7/22.
//

#include <libpcev/net/Connector.hpp>

using namespace pcev::net;
using namespace pcev::event;

Connector::Connector(EventLoop *loop, int connector_fd, bool need_conn)
    : owner_loop_(loop), connector_fd_(connector_fd),
      need_conn_(need_conn)
{
    assert(owner_loop_);
    connector_ch_ = std::make_shared<Channel>(owner_loop_, connector_fd_);
}

int Connector::connect() {
    return sock_connect();
}

void Connector::send(const std::string &msg) {
    connector_ch_->send(msg);
}

int Connector::sock_connect() {
    if (need_conn_) {
        connector_ch_->set_channel_type(ChannelType::K_CONNECT_CHANNEL);
    } else {
        connector_ch_->set_channel_type(ChannelType::K_NORMAL_CHANNEL);
    }
    connector_ch_->enable_write();
    return 0;
}