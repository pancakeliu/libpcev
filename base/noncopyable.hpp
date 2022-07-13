//
// Created by liupengcheng on 2021/6/29.
//

#ifndef LIBPCEV_NONCOPYABLE_HPP
#define LIBPCEV_NONCOPYABLE_HPP

namespace pcev
{

    class noncopyable
    {
    public:
        noncopyable(const noncopyable &) = delete;
        void operator=(const noncopyable &) = delete;

    protected:
        noncopyable() = default;
        ~noncopyable() = default;
    };

} // namespace libpcev

#endif // LIBPCEV_NON_COPYABLE_H
