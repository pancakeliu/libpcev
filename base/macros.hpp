//
// Created by liupengcheng on 2021/2/1.
//

#ifndef LIBPCEV_MACROS_HPP
#define LIBPCEV_MACROS_HPP

namespace pcev {

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

} // namespace pcev

#endif //LIBPCEV_MACROS_HPP
