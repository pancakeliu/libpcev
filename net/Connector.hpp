//
// Created by liupengcheng on 2021/7/22.
//

#ifndef LIBPCEV_CONNECTOR_HPP
#define LIBPCEV_CONNECTOR_HPP

#include <libpcev/base/noncopyable.hpp>
#include <libpcev/event/header.hpp>

#include <memory>

namespace pcev
{
namespace net
{

    class Connector : pcev::noncopyable
    {
    public:
        Connector() = delete;
        Connector(pcev::event::EventLoop *loop, int connector_fd, bool need_conn);
        ~Connector() = default;

        int connect();

        void set_connect_callback(pcev::event::ConnectCallback &connect_cb) {
            connector_ch_->set_connect_callback(connect_cb);
        }
        void set_message_callback(pcev::event::MessageCallback &message_cb) {
            connector_ch_->set_read_callback(message_cb);
        }
        void set_write_complete_callback(pcev::event::WriteCompleteCallback &write_complete_cb) {
            connector_ch_->set_write_callback(write_complete_cb);
        }
        void set_close_callback(pcev::event::CloseCallback &close_cb) {
            connector_ch_->set_close_callback(close_cb);
        }
        void set_error_callback(pcev::event::ErrorCallback &error_cb) {
            connector_ch_->set_error_callback(error_cb);
        }

        void send(const std::string &msg);

    private:
        int sock_connect();

    private:
        pcev::event::EventLoop                *owner_loop_;
        int                                    connector_fd_;
        std::shared_ptr<pcev::event::Channel>  connector_ch_;
        bool                                   need_conn_;
    };

} // namespace net
} // namespace pcev

#endif //LIBPCEV_CONNECTOR_HPP
