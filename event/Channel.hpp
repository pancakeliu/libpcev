//
// Created by liupengcheng on 2021/6/29.
//

#ifndef LIBPCEV_CHANNEL_HPP
#define LIBPCEV_CHANNEL_HPP

#include <libpcev/base/noncopyable.hpp>
#include <libpcev/base/Buffer.hpp>
#include <libpcev/net/InetAddress.hpp>
#include <libpcev/event/Callbacks.hpp>

#include <functional>
#include <memory>
#include <list>
#include <cerrno>

#include <sys/epoll.h>

namespace pcev
{
namespace event
{

    class EventLoop;

    // NOTE: no such EPOLL_EVENTS defined in sys/epoll.h
    const int K_NO_EVENT    = 0x0000;
    const int K_EVENT_READ  = EPOLLIN;
    const int K_EVENT_WRITE = EPOLLOUT;

    enum ChannelType {
        K_NO_CHANNEL_TYPE,
        K_ACCEPT_CHANNEL,
        // connect channel just do once
        K_CONNECT_CHANNEL,
        K_NORMAL_CHANNEL,
    };

    enum ChannelStats {
        K_CHANNEL_NOT_SET,
        K_CHANNEL_CLOSED,
        K_CHANNEL_READY,
    };

    enum NetworkStats {
        K_NETWORK_NOT_SET,
        K_NETWORK_RECV,
        K_NETWORK_SEND,
        K_NETWORK_CLOSE,
    };

    class Channel
    {
    private:
        using WriteQueue = std::list<pcev::base::OffsetBuffer *>;

    public:
        Channel(EventLoop *loop, int fd);
        ~Channel();

        void reset();

        void in_pool() {
            in_pool_ = true;
        }

        bool is_in_pool() const {
            return in_pool_;
        }

        void set_fd(int fd) {
            fd_ = fd;
        }
        int fd() const {
            return fd_;
        }
        void set_events(int events) {
            events_ = events;
        }
        int events() const {
            return events_;
        }

        void add_revents(int revents) {
            revents_ = revents_ | revents;
        }
        int revents() const {
            return revents_;
        }
        void clear_revents() {
            revents_ = K_NO_EVENT;
        }

        bool is_none_event() const {
            return !events_;
        }

        int index() const {
            return index_;
        }
        void incr_index() {
            if (index_ < 0) {
                ++index_;
            }
        }

        void enable_read() {
            events_ = events_ | K_EVENT_READ;
            update();
        }
        void enable_write() {
            events_ = events_ | K_EVENT_WRITE;
            update();
        }
        void disable_read() {
            events_ = events_ & ~K_EVENT_READ;
            update();
        }
        void disable_write() {
            events_ = events_ & ~K_EVENT_WRITE;
            update();
        }

        const AcceptCallback &accept_callback() const {
            return accept_callback_;
        }
        const MessageCallback &read_callback() const {
            return read_callback_;
        }
        const WriteCompleteCallback &write_callback() const {
            return write_callback_;
        }
        const ConnectCallback &connect_callback() const {
            return connect_callback_;
        }
        const ErrorCallback &error_callback() const {
            return error_callback_;
        }
        const CloseCallback &close_callback() const {
            return close_callback_;
        }

        void set_accept_callback(const AcceptCallback &cb) {
            accept_callback_ = cb;
        }
        void set_read_callback(const MessageCallback &cb) {
            read_callback_ = cb;
        }
        void set_write_callback(const WriteCompleteCallback &cb) {
            write_callback_ = cb;
        }
        void set_connect_callback(const ConnectCallback &cb) {
            connect_callback_ = cb;
        }
        void set_error_callback(const ErrorCallback &cb) {
            error_callback_ = cb;
        }
        void set_close_callback(const CloseCallback &cb) {
            close_callback_ = cb;
        }

        void fork_callbacks(Channel *other_ch);

        void set_channel_stats(const ChannelStats ch_stats) {
            ch_stats_ = ch_stats;
        }
        ChannelStats channel_stats() const {
            return ch_stats_;
        }

        void set_channel_type(const ChannelType ch_type) {
            ch_type_ = ch_type;
        }
        ChannelType channel_type() const {
            return ch_type_;
        }

        void set_errno() {
            errno_ = errno;
        }
        int get_errno() const {
            return errno_;
        }

        void set_network_stats(const NetworkStats ns) {
            ns_ = ns;
        }
        NetworkStats network_stats() const {
            return ns_;
        }

        void set_local_addr(pcev::net::InetAddress *laddr) {
            local_addr_ = laddr;
        }
        void set_peer_addr(pcev::net::InetAddress *paddr) {
            peer_addr_ = paddr;
        }

        pcev::net::InetAddress *local_addr() const {
            return local_addr_;
        }
        pcev::net::InetAddress *peer_addr() const {
            return peer_addr_;
        }

        pcev::base::Buffer *read_buf() {
            return read_buf_;
        }
        WriteQueue &write_queue() {
            return write_queue_;
        }
        bool write_queue_empty() const {
            return write_queue_.empty();
        }

        std::string &pending_buf() {
            return pending_buf_;
        }

        // Channel::update() ==> EventLoop::update_channel() ==> Poller::update_channel()
        void update();

        void send(const std::string &msg);

    private:
        // owner_loop_ owns this Channel
        EventLoop *owner_loop_;

        bool in_pool_;
        int  fd_;
        int  events_;
        // it's the received event types of epoll
        int  revents_;

        int index_;

        ChannelStats ch_stats_;
        ChannelType  ch_type_;
        NetworkStats ns_;
        int          errno_{};

        AcceptCallback        accept_callback_;
        MessageCallback       read_callback_;
        WriteCompleteCallback write_callback_;
        ConnectCallback       connect_callback_;
        ErrorCallback         error_callback_;
        CloseCallback         close_callback_;

        pcev::net::InetAddress *local_addr_{};
        pcev::net::InetAddress *peer_addr_{};
        pcev::base::Buffer     *read_buf_;
        WriteQueue              write_queue_;

        std::string             pending_buf_;
    };

} // namespace event
} // namespace libpcev

#endif //LIBPCEV_CHANNEL_HPP
