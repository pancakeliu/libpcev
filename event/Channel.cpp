//
// Created by liupengcheng on 2021/6/29.
//

#include <libpcev/event/header.hpp>

#include <cstring>

using namespace pcev::event;
using namespace pcev::base;

Channel::Channel(EventLoop *loop, int fd) :
    owner_loop_(loop), fd_(fd), events_(0), index_(-1), in_pool_(false),
    revents_(K_NO_EVENT), ch_stats_(ChannelStats::K_CHANNEL_NOT_SET),
    ch_type_(ChannelType::K_NO_CHANNEL_TYPE), ns_(NetworkStats::K_NETWORK_NOT_SET)
{
    read_buf_ = owner_loop_->buffer_pool()->get_obj();
    assert(read_buf_);
}

Channel::~Channel() {
    owner_loop_->buffer_pool()->return_obj(read_buf_);
    read_buf_ = nullptr;
    for (auto &it : write_queue_) {
        owner_loop_->offset_buffer_pool()->return_obj(it);
    }

    if (local_addr_) {
        delete local_addr_;
        local_addr_ = nullptr;
    }
    if (peer_addr_) {
        delete peer_addr_;
        peer_addr_ = nullptr;
    }
}

void Channel::reset() {
    owner_loop_ = nullptr;
    fd_         = -1;
    index_      = -1;
    events_     = K_NO_EVENT;
    revents_    = K_NO_EVENT;
    ch_type_    = ChannelType::K_NO_CHANNEL_TYPE;
    ns_         = NetworkStats::K_NETWORK_NOT_SET;
    ch_stats_   = ChannelStats::K_CHANNEL_NOT_SET;
    errno_      = 0;

    accept_callback_  = nullptr;
    read_callback_    = nullptr;
    write_callback_   = nullptr;
    connect_callback_ = nullptr;
    error_callback_   = nullptr;
    close_callback_   = nullptr;

    owner_loop_->inetaddr_pool()->return_obj(local_addr_);
    owner_loop_->inetaddr_pool()->return_obj(peer_addr_);

    local_addr_ = nullptr;
    peer_addr_  = nullptr;

    read_buf_->reset();
    for (auto &it : write_queue_) {
        owner_loop_->offset_buffer_pool()->return_obj(it);
    }
    write_queue_.clear();
}

// TODO: enable_share_form_this
void Channel::update() {
    if (events_ == K_NO_EVENT) {
        owner_loop_->close_channel(this);
        return;
    }
    owner_loop_->update_channel(this);
}

void Channel::fork_callbacks(Channel *other_ch) {
    if (accept_callback_) {
        other_ch->set_accept_callback(accept_callback_);
    }
    if (read_callback_) {
        other_ch->set_read_callback(read_callback_);
    }
    if (write_callback_) {
        other_ch->set_write_callback(write_callback_);
    }
    if (connect_callback_) {
        other_ch->set_connect_callback(connect_callback_);
    }
    if (close_callback_) {
        other_ch->set_close_callback(close_callback_);
    }
    if (error_callback_) {
        other_ch->set_error_callback(error_callback_);
    }
}

void Channel::send(const std::string &msg) {
    auto buffer_cnt = (msg.size() % K_DEFAULT_BUFFER_SIZE == 0) ?
            (msg.size() / K_DEFAULT_BUFFER_SIZE) : ((msg.size() / K_DEFAULT_BUFFER_SIZE) + 1);
    for (int i = 0; i < buffer_cnt; ++i) {
        auto ob = owner_loop_->offset_buffer_pool()->get_obj();
        auto s_msg = msg.substr(i * K_DEFAULT_BUFFER_SIZE, K_DEFAULT_BUFFER_SIZE);
        strncpy(ob->base(), s_msg.c_str(), s_msg.size());
        ob->set_len(s_msg.size());

        write_queue_.push_back(ob);
    }

    enable_write();
}