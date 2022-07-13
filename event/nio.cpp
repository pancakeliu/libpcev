//
// Created by liupengcheng on 2021/7/6.
//

#include <libpcev/event/header.hpp>
#include <libpcev/base/Buffer.hpp>
#include <libpcev/net/InetAddress.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include <cerrno>

using namespace pcev::io;
using namespace pcev::base;
using namespace pcev::net;
using namespace pcev::event;

void nio::set_loop(EventLoop *loop) {
    loop_ = loop;
}

void nio::nio_accept(Channel *ch) {
    InetAddress *peer_addr = loop_->inetaddr_pool()->get_obj();
    socklen_t addr_len = sizeof(InetAddress);

    int conn_fd = accept(ch->fd(), const_cast<struct sockaddr *>(peer_addr->get_sockaddr()), &addr_len);
    if (conn_fd < 0) {
        if (errno == EAGAIN) {
            return;
        } else {
            loop_->close_channel(ch);
            return;
        }
    }

    InetAddress *local_addr = loop_->inetaddr_pool()->get_obj();
    addr_len = sizeof(InetAddress);
    getsockname(ch->fd(), const_cast<struct sockaddr *>(local_addr->get_sockaddr()), &addr_len);
    Channel *cli_ch = loop_->channel_pool()->get_ch();

    fcntl(conn_fd, F_SETFL, fcntl(conn_fd, F_GETFL) | O_NONBLOCK);

    ch->fork_callbacks(cli_ch);
    cli_ch->set_fd(conn_fd);
    cli_ch->set_peer_addr(peer_addr);
    cli_ch->set_local_addr(local_addr);
    cli_ch->set_channel_type(ChannelType::K_NORMAL_CHANNEL);
    cli_ch->enable_read();

    if (cli_ch->accept_callback()) {
        cli_ch->accept_callback()(cli_ch);
    }
}

void nio::nio_read(Channel *ch) {
    for (;;) {
        size_t len = ch->read_buf()->capacity();
        auto nread = static_cast<size_t>(read(ch->fd(), ch->read_buf()->base(), len));
        if (nread < 0) {
            if (errno == EAGAIN) {
                return;
            } else {
                ch->set_errno();
                loop_->close_channel(ch);
                return;
            }
        }
        if (nread == 0) {
            loop_->close_channel(ch);
            return;
        }

        ch->read_buf()->set_used(nread);
        if (ch->read_callback()) {
            ch->read_callback()(ch, ch->read_buf());
        }
        if (nread != len) {
            return;
        }
    }
}

void nio::nio_write(Channel *ch) {
    for (;;) {
        if (ch->write_queue().empty()) {
            if (ch->network_stats() == NetworkStats::K_NETWORK_CLOSE) {
                ch->set_network_stats(NetworkStats::K_NETWORK_CLOSE);
                loop_->close_channel(ch);
            }
            return;
        }
        OffsetBuffer *ob = ch->write_queue().front();
        char *buf = ob->base() + ob->offset();
        size_t len = ob->len() - ob->offset();

        auto nwrite = static_cast<size_t>(write(ch->fd(), buf, len));
        if (nwrite < 0) {
            if (errno == EAGAIN) {
                return;
            } else {
                ch->set_errno();
                loop_->close_channel(ch);
                return;
            }
        }

        // the results are not specified
        if (nwrite == 0) {
            loop_->close_channel(ch);
            return;
        }

        if (ch->write_callback()) {
            ch->write_callback()(ch);
        }
        ob->set_offset(ob->offset() + nwrite);
        if (nwrite == len) {
            loop_->offset_buffer_pool()->return_obj(ob);
            ch->write_queue().pop_front();
        }
    }
}

void nio::nio_connect(Channel *ch) {
    InetAddress *peer_addr = loop_->inetaddr_pool()->get_obj();
    socklen_t addr_len = sizeof(InetAddress);
    int ret = getpeername(ch->fd(), const_cast<struct sockaddr *>(peer_addr->get_sockaddr()), &addr_len);
    if (ret < 0) {
        ch->set_errno();
        loop_->close_channel(ch);
        return;

    } else {
        InetAddress *local_addr = loop_->inetaddr_pool()->get_obj();
        addr_len = sizeof(InetAddress);
        getsockname(ch->fd(), const_cast<struct sockaddr *>(local_addr->get_sockaddr()), &addr_len);

        ch->set_peer_addr(peer_addr);
        ch->set_local_addr(local_addr);

        if (ch->connect_callback()) {
            ch->connect_callback()(ch);
        }
    }
}