//
// Created by liupengcheng on 2021/7/5.
//

#include <libpcev/net/Accepter.hpp>

using namespace pcev::net;
using namespace pcev::event;

Accepter::Accepter(pcev::event::EventLoop *loop, int accepter_fd)
    : owner_loop_(loop),
      accepter_fd_(accepter_fd)
{
    assert(loop != nullptr);
    accepter_ch_ = std::make_shared<Channel>(owner_loop_, accepter_fd_);
}

int Accepter::accept() {
    return sock_accept();
}

int Accepter::sock_accept() {
    accepter_ch_->set_channel_type(ChannelType::K_ACCEPT_CHANNEL);
    accepter_ch_->enable_read();
    return 0;
}