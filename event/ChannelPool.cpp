//
// Created by liupengcheng on 2021/7/25.
//

#include <libpcev/event/header.hpp>
#include <libpcev/base/macros.hpp>

#include <cassert>
#include <utility>

using namespace pcev::event;

static const int K_DEFAULT_CHANNEL_BLOCK_SIZE = 4096;

ChannelPool::ChannelPool(EventLoop *loop) {
    loop_     = loop;
    assert(loop_);
    capacity_ = K_DEFAULT_CHANNEL_BLOCK_SIZE;
    used_     = 0;

    for (int i = 0; i < capacity_; ++i) {
        auto *ch = new Channel(loop_, -1);
        ch->in_pool();
        ch_pool_.emplace_back(ch);
    }
}

ChannelPool::~ChannelPool() {
    for (auto &iter : ch_pool_) {
        delete iter;
        iter = nullptr;
    }
}

Channel *ChannelPool::get_ch() {
    if (unlikely(used_ == capacity_)) {
        for (int i = 0; i < K_DEFAULT_CHANNEL_BLOCK_SIZE; ++i) {
            auto *ch = new Channel(loop_, -1);
            ch_pool_.emplace_front(ch);
        }
        capacity_ = capacity_ + K_DEFAULT_CHANNEL_BLOCK_SIZE;
    }
    Channel *ret_ch = ch_pool_.front();
    ch_pool_.pop_front();
    ++used_;

    return ret_ch;
}

void ChannelPool::return_ch(Channel *ch) {
    ch->reset();
    ch->in_pool();

    ch_pool_.emplace_back(ch);
    --used_;
}