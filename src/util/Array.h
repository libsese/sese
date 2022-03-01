#pragma once

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
            return values[i];
        }

        const T &operator[](int i) const {
            return values[i];
        }
    };
}// namespace sese