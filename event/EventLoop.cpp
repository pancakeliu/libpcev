//
// Created by liupengcheng on 2021/6/29.
//

#include <libpcev/event/EventLoop.hpp>
#include <libpcev/event/Epoller.hpp>
#include <libpcev/event/Channel.hpp>
#include <libpcev/event/ChannelPool.hpp>
#include <libpcev/event/nio.hpp>

#include <iostream>
#include <cassert>

using namespace pcev::event;
using namespace pcev::base;
using namespace pcev::net;

EventLoop::EventLoop():
    quit_(false), is_loopping_(false),
    max_wait_time_(10000)
{
    ia_pool_ = new ObjectPool<InetAddress>;
    assert(ia_pool_);
    buf_pool_ = new ObjectPool<Buffer>;
    assert(buf_pool_);
    ob_pool_ = new ObjectPool<OffsetBuffer>;
    assert(ob_pool_);
    event_io_ = new pcev::io::nio;
    assert(event_io_);
}

EventLoop::~EventLoop() {
    delete epoller_ptr_;

    if (ia_pool_) {
        delete ia_pool_;
        ia_pool_ = nullptr;
    }
    if (ch_pool_) {
        delete ch_pool_;
        ch_pool_ = nullptr;
    }
    if (buf_pool_) {
        delete buf_pool_;
        buf_pool_ = nullptr;
    }
    if (event_io_) {
        delete event_io_;
        event_io_ = nullptr;
    }
}

void EventLoop::init() {
    epoller_ptr_ = new Epoller(this);
    assert(epoller_ptr_);
    event_io_->set_loop(this);
    ch_pool_ = new ChannelPool(this);
    assert(ch_pool_);
}

// TODO:这里后续需要调优
void EventLoop::loop() {
    assert(!is_loopping_);

    is_loopping_ = true;
    quit_ = false;

    while (!quit_) {
        active_channels_.clear();

        // TODO: 使用最小堆数据结构，取出最小的超时时间，作为epoll的阻塞时间
        epoller_ptr_->poll_events(max_wait_time_, &active_channels_);
        deal_events();

        // TODO: deal common events
        deal_common_events();
    }

    is_loopping_ = false;
}

void EventLoop::update_channel(Channel *ch) {
    epoller_ptr_->update_channel(ch);
}

void EventLoop::close_channel(Channel *ch) {
    if (ch->channel_stats() == ChannelStats::K_CHANNEL_CLOSED) {
        return;
    }
    ch->set_channel_stats(ChannelStats::K_CHANNEL_CLOSED);
    epoller_ptr_->remove_channel(ch);
    if (ch->close_callback()) {
        ch->close_callback()(ch);
    }
}

void EventLoop::deal_events() {
    for (auto &ch : active_channels_) {
        if ((ch->events() & K_EVENT_READ) && (ch->revents() & K_EVENT_READ)) {
            if (ch->channel_type() == ChannelType::K_ACCEPT_CHANNEL) {
                event_io_->nio_accept(ch);
            } else {
                event_io_->nio_read(ch);
            }
        }

        if ((ch->events() & K_EVENT_WRITE) && (ch->revents() & K_EVENT_WRITE)) {
            // NOTE: del K_EVENT_WRITE, if write_queue empty
            if (ch->write_queue_empty()) {
                ch->disable_write();
            }
            if (ch->channel_type() == ChannelType::K_CONNECT_CHANNEL) {
                // TODO: NOTE update channel type
                ch->set_channel_type(ChannelType::K_NORMAL_CHANNEL);
                event_io_->nio_connect(ch);
            } else {
                event_io_->nio_write(ch);
            }
        }

        if (ch->get_errno()) {
            if (ch->error_callback()) {
                ch->error_callback()(ch);
            }
            ch_pool_->return_ch(ch);
        }

        if (ch->channel_stats() == ChannelStats::K_CHANNEL_CLOSED && ch->is_in_pool()) {
            ch_pool_->return_ch(ch);
        }

        ch->clear_revents();
    }
}

void EventLoop::regist_common_cb(std::string cb_key, CommonCallback &common_callback)
{
    CommonCallback *common_cb_ptr = &common_callback;

    assert(common_callbacks_.find(cb_key) != common_callbacks_.end());
    common_callbacks_.emplace(cb_key, common_callback);
}

void EventLoop::unregist_common_cb(std::string cb_key)
{
    common_callbacks_.erase(cb_key);
}

void EventLoop::deal_common_events()
{
    for (auto &common_cb : common_callbacks_) {
        common_cb.second();
    }
}