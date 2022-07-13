//
// Created by liupengcheng on 2021/6/29.
//

#ifndef LIBPCEV_EVENTLOOP_HPP
#define LIBPCEV_EVENTLOOP_HPP

#include <libpcev/event/Callbacks.hpp>
#include <libpcev/base/ObjectPool.hpp>
#include <libpcev/base/Buffer.hpp>
#include <libpcev/net/InetAddress.hpp>

#include <vector>
#include <map>
#include <memory>

namespace pcev
{
namespace io
{
    class nio;
} // namespace io

namespace event
{

    class Channel;
    class ChannelPool;
    class Epoller;

    class EventLoop
    {
    private:
        using ChannelVec = std::vector<Channel *>;
    public:
        EventLoop();
        ~EventLoop();

        void init();

        // precondition:
        // 1. should be IO thread
        // 2. can not call loop() repeated
        void loop();

        void update_channel(Channel *ch);
        void close_channel(Channel *ch);

        void quit() {
            quit_ = true;
        }

        ChannelPool *channel_pool() {
            return ch_pool_;
        }
        pcev::base::ObjectPool<pcev::net::InetAddress> *inetaddr_pool() {
            return ia_pool_;
        }
        pcev::base::ObjectPool<pcev::base::Buffer> *buffer_pool() {
            return buf_pool_;
        }
        pcev::base::ObjectPool<pcev::base::OffsetBuffer> *offset_buffer_pool() {
            return ob_pool_;
        }

    public:
        void regist_common_cb(std::string cb_key, CommonCallback &common_callback);
        void unregist_common_cb(std::string cb_key);

    private:
        void deal_events();
        void deal_common_events();

    private:
        ChannelVec active_channels_;

        bool  quit_;
        bool  is_loopping_;
        int   max_wait_time_;

        Epoller        *epoller_ptr_;
        pcev::io::nio  *event_io_;

        ChannelPool                                        *ch_pool_;
        pcev::base::ObjectPool<pcev::net::InetAddress>     *ia_pool_;
        pcev::base::ObjectPool<pcev::base::Buffer>         *buf_pool_;
        pcev::base::ObjectPool<pcev::base::OffsetBuffer>   *ob_pool_;

        std::map<std::string, CommonCallback *>             common_callbacks_;
    };

} // namespace event
} // namespace pcev

#endif //LIBPCEV_EVENTLOOP_HPP
