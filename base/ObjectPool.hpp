//
// Created by liupengcheng on 2021/7/25.
//

#ifndef LIBPCEV_MEMPOOL_H
#define LIBPCEV_MEMPOOL_H

#include <libpcev/base/noncopyable.hpp>
#include <libpcev/base/macros.hpp>

#include <cstdint>
#include <list>

namespace pcev
{
namespace base
{

    template <class T, int block_size = 4096>
    class ObjectPool : pcev::noncopyable
    {
    public:
        ObjectPool() {
            used_ = 0;
            for (int i = 0; i < capacity_; ++i) {
                T *obj = new T;
                pool_.emplace_back(std::move(obj));
            }
        }

        ~ObjectPool() {
            for (auto &iter : pool_) {
                delete iter;
                iter = nullptr;
            }
        }

        T *get_obj() {
            if (unlikely(used_ == capacity_)) {
                for (int i = 0; i < block_size; ++i) {
                    T *obj = new T;
                    pool_.emplace_front(std::move(obj));
                }
                capacity_ = capacity_ + block_size;
            }
            T *ret_obj = pool_.front();
            pool_.pop_front();
            ++used_;

            return ret_obj;
        }

        void return_obj(T *obj) {
            obj->reset();
            pool_.emplace_back(std::move(obj));
            --used_;
        }

    private:
        std::list<T*>   pool_;
        int             used_;
        int             capacity_{};
    };

} // namespace base
} // namespace pcev

#endif //LIBPCEV_MEMPOOL_H
