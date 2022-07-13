//
// Created by liupengcheng on 2021/2/1.
//

#ifndef LIBPCEV_CALLBACKS_HPP
#define LIBPCEV_CALLBACKS_HPP

#include <libpcev/base/Buffer.hpp>

#include <functional>
#include <memory>

namespace pcev
{
namespace event
{
    class Channel;
    using TcpConnectionPtr      = Channel*;

    using CommonCallback        = std::function<void()>;
    using AcceptCallback        = std::function<void (const TcpConnectionPtr &)>;
    using ConnectCallback       = std::function<void (const TcpConnectionPtr &)>;
    using CloseCallback         = std::function<void (const TcpConnectionPtr &)>;
    using WriteCompleteCallback = std::function<void (const TcpConnectionPtr &)>;
    using ErrorCallback         = std::function<void (const TcpConnectionPtr &)>;

    using MessageCallback       = std::function<void (const TcpConnectionPtr &,
                                                      pcev::base::Buffer *)>;

} // namepace event
} // namespace pcev

#endif //LIBPCEV_CALLBACKS_HPP
