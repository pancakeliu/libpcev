//
// Created by liupengcheng on 2021/7/6.
//

#ifndef LIBPCEV_NIO_HPP
#define LIBPCEV_NIO_HPP

#include <memory>

namespace pcev
{
namespace event
{
    class Channel;
    class EventLoop;
} // namespace event

namespace io
{

    class nio {
    public:
        nio() = default;
        ~nio() = default;

        void set_loop(pcev::event::EventLoop *loop);

        void nio_accept(pcev::event::Channel *ch);
        void nio_read(pcev::event::Channel *ch);
        void nio_write(pcev::event::Channel *ch);
        void nio_connect(pcev::event::Channel *ch);

    private:
        pcev::event::EventLoop *loop_;
    };

} // namespace io
} // namespace pcev

#endif //LIBPCEV_NIO_HPP
