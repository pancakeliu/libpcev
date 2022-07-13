//
// Created by liupengcheng on 2021/7/5.
//

#ifndef LIBPCEV_ACCEPTER_HPP
#define LIBPCEV_ACCEPTER_HPP

#include <libpcev/base/noncopyable.hpp>
#include <libpcev/event/header.hpp>

#include <memory>

namespace pcev
{
namespace net
{
    class Accepter : public noncopyable
    {
    public:
        Accepter() = delete;
        Accepter(pcev::event::EventLoop *loop, int accepter_fd);
        ~Accepter() = default;

        void set_accept_callback(pcev::event::AcceptCallback &accpet_cb) {
            accepter_ch_->set_accept_callback(accpet_cb);
        }
        void set_message_callback(pcev::event::MessageCallback &message_cb) {
            accepter_ch_->set_read_callback(message_cb);
        }
        void set_write_complete_callback(pcev::event::WriteCompleteCallback &write_complete_cb) {
            accepter_ch_->set_write_callback(write_complete_cb);
        }
        void set_close_callback(pcev::event::CloseCallback &close_cb) {
            accepter_ch_->set_close_callback(close_cb);
        }
        void set_error_callback(pcev::event::ErrorCallback &error_cb) {
            accepter_ch_->set_error_callback(error_cb);
        }

        int accept();

    private:
        int sock_accept();

    private:
        pcev::event::EventLoop                *owner_loop_;
        int                                    accepter_fd_;
        std::shared_ptr<pcev::event::Channel>  accepter_ch_;
    };

} // namespace net
} // namespace pcev

#endif //LIBPCEV_ACCEPTER_HPP
