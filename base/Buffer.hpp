//
// Created by liupengcheng on 2021/7/15.
//

#ifndef LIBPCEV_BUFFER_HPP
#define LIBPCEV_BUFFER_HPP

#include <libpcev/base/noncopyable.hpp>

#include <cstddef>
#include <cassert>
#include <string>

namespace pcev
{
namespace base
{

    const size_t K_DEFAULT_BUFFER_SIZE = 16384;

    class Buffer : pcev::noncopyable
    {
    public:
        Buffer() noexcept;
        ~Buffer();

        char *base() const {
            return base_;
        }
        size_t capacity() const {
            return capacity_;
        }
        void set_used(size_t used) {
            used_ = used;
        }
        size_t used() const {
            return used_;
        }

        void reset();

        std::string to_string() const {
            return std::string(base_, used_);
        }

    public:
        char *tail() const {
            return base_ + used_;
        }

        int append(const char *data, size_t size);

        int append(const char *data);

        int add_used(size_t used);

    private:
        char   *base_;
        size_t  capacity_;
        size_t  used_;

    }; // class Buffer

    class OffsetBuffer : pcev::noncopyable
    {
    public:
        OffsetBuffer() noexcept;
        ~OffsetBuffer();

        char *base() const {
            return base_;
        };
        size_t capacity() const {
            return capacity_;
        }
        size_t len() const {
            return len_;
        }
        size_t offset() const {
            return offset_;
        }

        void set_len(size_t len) {
            len_ = len;
        }
        void set_offset(size_t offset) {
            offset_ = offset;
        }

        void reset();

    private:
        char   *base_;
        size_t  capacity_;
        size_t  len_;
        size_t  offset_;
    }; // class OffsetBuffer

} // namespace base
} // namespace pcev

#endif //LIBPCEV_BUFFER_HPP
