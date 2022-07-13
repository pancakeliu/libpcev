//
// Created by liupengcheng on 2021/7/4.
//

#include <libpcev/net/InetAddress.hpp>

#include <cstring>
#include <cstdio>
#include <cassert>

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <iostream>

using namespace pcev::net;

// len(max port) 5 + len(\0) 1 = 6
static const int PORT_STR_SIZE = 6;

InetAddress::InetAddress() {
    bzero(&addr_, sizeof(addr_));
}

InetAddress::InetAddress(uint16_t port, bool is_loopback) {
    bzero(&addr_, sizeof(addr_));
    auto ip = static_cast<in_addr_t>(is_loopback ? INADDR_LOOPBACK : INADDR_ANY);
    addr_.sin_family      = AF_INET;
    addr_.sin_addr.s_addr = htonl(ip);
    addr_.sin_port        = htons(port);
}

InetAddress::InetAddress(std::string ip, uint16_t port) {
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family      = AF_INET;
    assert(::inet_pton(AF_INET, ip.c_str(), &(addr_.sin_addr)) == 0);
    addr_.sin_port        = htons(port);
}

std::string InetAddress::to_ip() const {
    return inet_ntoa(*(
            const_cast<struct in_addr *>(
                    reinterpret_cast<const struct in_addr *>(&addr_.sin_addr.s_addr))));
}

std::string InetAddress::to_ip_port() const {
    std::string ip = inet_ntoa(*(
            const_cast<struct in_addr *>(
                    reinterpret_cast<const struct in_addr *>(&addr_.sin_addr.s_addr))));

    char port[PORT_STR_SIZE] = {};
    snprintf(port, PORT_STR_SIZE, "%d", ntohs(addr_.sin_port));

    return ip + ":" + port;
}

uint16_t InetAddress::to_port() const {
    return ntohs(addr_.sin_port);
}

bool InetAddress::resolve(std::string hostname, InetAddress *out) {
    assert(out);

    struct hostent  hent;
    struct hostent *he = nullptr;
    int herrno = 0;
    bzero(&hent, sizeof(hent));

    char t_resolve_buffer[64 * 1024] = {};
    int ret = gethostbyname_r(hostname.c_str(), &hent, t_resolve_buffer,
                              sizeof t_resolve_buffer, &he, &herrno);
    if (ret != 0 || he == nullptr) {
        return false;
    }
    assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
    out->addr_.sin_addr = *reinterpret_cast<struct in_addr *>(he->h_addr);

    return true;
}