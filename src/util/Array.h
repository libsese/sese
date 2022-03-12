#pragma once
#include "IndexOutOfBoundsException.h"

namespace sese {

    template<typename T, int n>
    class Array {
    private:
        T values[n]{0};
        int len = n;

    public:
        [[nodiscard]] int length() const noexcept { return this->len; }

    public:
        T &operator[](int i) {
            if(len <= i || i < 0) throw IndexOutOfBoundsException();
            return values[i];
        }

        const T &operator[](int i) const {
            if(len <= i || i < 0) throw IndexOutOfBoundsException();
            return values[i];
        }
    };
}// namespace sese