//
// Created by liupengcheng on 2021/6/29.
//

#include <libpcev/event/header.hpp>
#include <libpcev/base/macros.hpp>

#include <cstring>
#include <strings.h>

using namespace pcev::event;

const int EPOLL_EVENTS_INIT_SIZE = 64;

Epoller::Epoller(EventLoop *loop) : owner_loop_(loop) {
    ep_ctx_ptr_ = new epoll_ctx_t;
    ep_ctx_ptr_->epfd = epoll_create(EPOLL_EVENTS_INIT_SIZE);
    assert(ep_ctx_ptr_->epfd != -1);
    ep_ctx_ptr_->events = static_cast<epoll_event*>
            (malloc(sizeof(struct epoll_event) * EPOLL_EVENTS_INIT_SIZE));
    ep_ctx_ptr_->event_size = 0;
    assert(ep_ctx_ptr_->events);
    bzero(ep_ctx_ptr_->events, sizeof(struct epoll_event) * EPOLL_EVENTS_INIT_SIZE);
}

Epoller::~Epoller() {
    if (ep_ctx_ptr_) {
        if (ep_ctx_ptr_->events) {
            free(ep_ctx_ptr_->events);
            ep_ctx_ptr_->events = nullptr;
        }
        delete ep_ctx_ptr_;
        ep_ctx_ptr_ = nullptr;
    }
}

void Epoller::update_channel(Channel *ch) {
    int ep_ops;
    if (ch->index() < 0) {
        // this channel is a new one
        assert(channel_map_.find(ch->fd()) == channel_map_.end());
        ep_ops = EPOLL_CTL_ADD;
    } else {
        assert(channel_map_.find(ch->fd()) != channel_map_.end());
        ep_ops = EPOLL_CTL_MOD;
    }
    struct epoll_event ee;
    bzero(&ee, sizeof(ee));
    ee.data.fd = ch->fd();
    ee.events  = ch->events();

    assert(epoll_ctl(ep_ctx_ptr_->epfd, ep_ops, ee.data.fd, &ee) == 0);
    if (ep_ops == EPOLL_CTL_ADD) {
        ++ep_ctx_ptr_->event_size;
        channel_map_[ee.data.fd] = ch;
    } else if (ep_ops == EPOLL_CTL_MOD) {
        channel_map_[ee.data.fd]->set_fd(ee.data.fd);
        channel_map_[ee.data.fd]->set_events(ee.events);
    }
    ch->incr_index();
}

void Epoller::remove_channel(Channel *ch) {
    assert(channel_map_.find(ch->fd()) != channel_map_.end());
    struct epoll_event ee;
    bzero(&ee, sizeof(ee));
    ee.data.fd = ch->fd();
    ee.events = 0;

    assert(epoll_ctl(ep_ctx_ptr_->epfd, EPOLL_CTL_DEL, ee.data.fd, &ee) == 0);
    --ep_ctx_ptr_->event_size;
    channel_map_.erase(ch->fd());
}

void Epoller::poll_events(int max_wait_time, ChannelVec *active_channels) {
    assert(ep_ctx_ptr_);
    if (ep_ctx_ptr_->event_size == 0) {
        return;
    }

    int nevs = epoll_wait(ep_ctx_ptr_->epfd, ep_ctx_ptr_->events, ep_ctx_ptr_->event_size, max_wait_time);
    if (unlikely(nevs < 0)) {
        return;
    }
    if (nevs == 0) {
        return;
    }

    int nevents = 0;
    for (int i = 0; i < nevs; ++i) {
        struct epoll_event *ee = ep_ctx_ptr_->events + i;
        uint32_t revents = ee->events;
        if (revents) {
            ++nevents;
            Channel *ch = channel_map_[ee->data.fd];
            if (likely(ch)) {
                if (revents & (EPOLLIN | EPOLLHUP | EPOLLERR)) {
                    ch->add_revents(K_EVENT_READ);
                }
                if (revents & (EPOLLOUT | EPOLLHUP | EPOLLERR)) {
                    ch->add_revents(K_EVENT_WRITE);
                }
                active_channels->push_back(ch);
            }
        }
        if (nevents == nevs) {
            break;
        }
    }
}