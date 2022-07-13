//
// Created by liupengcheng on 2021/7/5.
//

#include <libpcev/net/Socket.hpp>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <cassert>
#include <cerrno>

using namespace pcev::net;

static const int BACKLOG_SZIE = 1024;

Socket::Socket(const InetAddress &addr) {
    addr_ = addr;
    sock_type_ = K_NOT_SET;
}

Socket::~Socket() {
    close_socket();
}

int Socket::tcp_server(SocketOptions opt) {
    assert(sock_type_ == K_NOT_SET);

    sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd_ < 0) {
        return SocketRC::SOCKET_ERROR;
    }

    fcntl(sock_fd_, F_SETFL, fcntl(sock_fd_, F_GETFL) | O_NONBLOCK);

    // NOTE: SO_REUSEADDR means that you can reuse sockaddr of TIME_WAIT status
    int reuse_addr = 1;
    if (setsockopt(sock_fd_, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)) < 0) {
        close(sock_fd_);
        return SocketRC::SET_SOCK_OPT_ERROR;
    }
    if (opt == SocketOptions::K_SOCKET_OPTION_REUSEPORT) {
        int reuse_port = 1;
        if (setsockopt(sock_fd_, SOL_SOCKET, SO_REUSEPORT, &reuse_port, sizeof(reuse_port)) < 0) {
            close(sock_fd_);
            return SocketRC::SET_SOCK_OPT_ERROR;
        }
    }

    if (bind(sock_fd_, addr_.get_sockaddr(), sizeof(addr_)) < 0) {
        close(sock_fd_);
        return SocketRC::BIND_ERROR;
    }
    sock_type_ = SockType::K_TCP_SERVER;

    if (listen(sock_fd_, BACKLOG_SZIE) != 0) {
        close(sock_fd_);
        return SocketRC::LISTEN_ERROR;
    }

    return SocketRC::SOCKET_RC_OK;
}

int Socket::tcp_client() {
    assert(sock_type_ == K_NOT_SET);

    sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd_ < 0) {
        return SocketRC::SOCKET_ERROR;
    }

    fcntl(sock_fd_, F_SETFL, fcntl(sock_fd_, F_GETFL) | O_NONBLOCK);

    int rc = connect(sock_fd_, addr_.get_sockaddr(), sizeof(addr_));
    if (rc < 0 && errno != EINPROGRESS) {
        close(sock_fd_);
        return SocketRC::CONNECT_ERROR;
    }
    sock_type_ = SockType::K_TCP_CLIENT;
    if (errno == EINPROGRESS) {
        return SocketRC::CONNECT_EINPROGRESS;
    }

    return SocketRC::SOCKET_RC_OK;
}

void Socket::close_socket() {
    if (sock_type_ != K_NOT_SET) {
        close(sock_fd_);
        sock_fd_ = -1;
        sock_type_ = SockType::K_NOT_SET;
    }
}