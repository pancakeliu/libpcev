//
// Created by liupengcheng on 2021/7/25.
//

#ifndef LIBPCEV_CHANNELPOOL_HPP
#define LIBPCEV_CHANNELPOOL_HPP

#include <libpcev/base/noncopyable.hpp>

#include <list>

namespace pcev
{
namespace event
{

    class EventLoop;
    class Channel;

    class ChannelPool : pcev::noncopyable
    {
    public:
        explicit ChannelPool(EventLoop *loop);
        ~ChannelPool();

        Channel *get_ch();
        void return_ch(Channel *obj);
    private:
        EventLoop            *loop_;
        std::list<Channel *>  ch_pool_;
        int                   capacity_;
        int                   used_;
    };

} // namespace event
} // namespace pcev

#endif //LIBPCEV_CHANNELPOOL_HPP
