//
// Created by liupengcheng on 2021/7/15.
//

#include <libpcev/base/Buffer.hpp>

#include <cassert>
#include <cstring>

using namespace pcev::base;

Buffer::Buffer() noexcept {
    base_ = new char[K_DEFAULT_BUFFER_SIZE];
    assert(base_);
    capacity_ = K_DEFAULT_BUFFER_SIZE;
    used_     = 0;
}

Buffer::~Buffer() {
    delete []base_;
    capacity_ = 0;
    used_     = 0;
}

void Buffer::reset() {
    used_ = 0;
}

int Buffer::append(const char *data, size_t size) {
    if (used_ + size > capacity_) {
        return -1;
    }
    std::memmove(tail(), data, size);
    used_ += size;

    return 0;
}

int Buffer::append(const char *data) {
    return append(data, std::strlen(data));
}

int Buffer::add_used(size_t used) {
    if (used_ + used > capacity_) {
        return -1;
    }
    used_ += used;

    return 0;
}

OffsetBuffer::OffsetBuffer() noexcept {
    base_ = new char[K_DEFAULT_BUFFER_SIZE];
    assert(base_);
    capacity_ = K_DEFAULT_BUFFER_SIZE;
    offset_   = 0;
    len_      = 0;
}

OffsetBuffer::~OffsetBuffer() {
    delete []base_;
    capacity_ = 0;
    offset_   = 0;
    len_      = 0;
}

void OffsetBuffer::reset() {
    offset_ = 0;
    len_    = 0;
}