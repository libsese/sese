#pragma once
#include "IndexOutOfBoundsException.h"

namespace sese {

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
            for(auto &i : v){
                this->values[index] = i;
                index += 1;
            }
        }

        [[nodiscard]] int length() const noexcept { return this->len; }

    public:
        T &operator[](int i) {
            if (len <= i || i < 0) throw IndexOutOfBoundsException();
            return values[i];
        }

        const T &operator[](int i) const {
            if (len <= i || i < 0) throw IndexOutOfBoundsException();
            return values[i];
        }
    };
}// namespace sese