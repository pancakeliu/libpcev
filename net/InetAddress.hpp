//
// Created by liupengcheng on 2021/7/4.
//

#ifndef LIBPCEV_INETADDRESS_HPP
#define LIBPCEV_INETADDRESS_HPP

#include <libpcev/base/copyable.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdint>
#include <string>

namespace pcev
{
namespace net
{

    class InetAddress : public pcev::copyable
    {
    public:
        InetAddress();
        explicit InetAddress(uint16_t port, bool is_loopback = false);

        InetAddress(std::string ip, uint16_t port);

        explicit InetAddress(const struct sockaddr_in &addr)
            : addr_(addr) {}

        sa_family_t family() const { return addr_.sin_family; }
        std::string to_ip() const;
        std::string to_ip_port() const;
        uint16_t    to_port() const;

        void reset() {}

        const struct sockaddr *get_sockaddr() const {
            return reinterpret_cast<const struct sockaddr *>(&addr_);
        }

        // resolve hostname to ip address, not changing port or sin_family
        // return true on success
        static bool resolve(std::string hostname, InetAddress *out);

    private:
        struct sockaddr_in addr_{};
    }; // class InetAddress

} // namespace net
} // namespace pcev

#endif //LIBPCEV_INETADDRESS_HPP
