//
// Created by liupengcheng on 2021/6/29.
//

#ifndef LIBPCEV_EPOLLER_HPP
#define LIBPCEV_EPOLLER_HPP

#include <libpcev/base/noncopyable.hpp>

#include <sys/epoll.h>
#include <memory>
#include <map>
#include <vector>

namespace pcev
{
namespace event
{

    class Channel;
    class EventLoop;

    class Epoller : pcev::noncopyable
    {
    private:
        struct epoll_ctx_t {
            int                 epfd;
            struct epoll_event *events;
            int                 event_size;
        };

    private:
        using ChannelVec = std::vector<Channel *>;
        // fd => Channel *
        using ChannelMap = std::map<int, Channel *>;

    public:
        explicit Epoller(EventLoop *loop);
        ~Epoller();

        void update_channel(Channel *ch);
        void remove_channel(Channel *ch);
        void poll_events(int max_wait_time, ChannelVec *active_channels);

    private:
        EventLoop    *owner_loop_;
        epoll_ctx_t  *ep_ctx_ptr_;
        ChannelMap    channel_map_;
    };

} // namespace event
} // namespace pcev

#endif //LIBPCEV_EPOLLER_HPP
