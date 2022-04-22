/**
 * @file Array.h
 * @brief 数组模板类
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once
#include "sese/IndexOutOfBoundsException.h"
#include "sese/Util.h"

#ifdef _WIN32
#pragma warning(disable : 4018)
#endif

namespace sese {

    /**
     * @brief 模板数组类
     * @tparam T 数据类型
     * @tparam n 数据数量
     */
    template<typename T, int n>
    class Array {
    private:
        T values[n]{0};
        int len = n;

    public:
        explicit Array() noexcept = default;

        Array(const std::initializer_list<T> &v) {
            if (v.size() > len) throw IndexOutOfBoundsException();
            int index = 0;
            for (auto &i: v) {
                this->values[index] = i;
                index += 1;
            }
        }

        [[nodiscard]] int length() const noexcept { return this->len; }

    public:
        T &operator[](int i) {
            // if (len <= i || i < 0) throw IndexOutOfBoundsException();
            if (!CheckRange(i, len)) throw IndexOutOfBoundsException();
            return values[i];
        }

        const T &operator[](int i) const {
            // if (len <= i || i < 0) throw IndexOutOfBoundsException();
            if (!CheckRange(i, len)) throw IndexOutOfBoundsException();
            return values[i];
        }
    };
}// namespace sese