#pragma once
#include <cstdlib>
#include <memory>

namespace sese {

    template<typename T>
    class HeapArray {
    public:
        T *values;
        int len = 0;

    public:
        typedef std::shared_ptr<HeapArray> Ptr;

        explicit HeapArray(int len) noexcept {
            this->len = len;
            this->values = new T[len];
        }

        ~HeapArray() noexcept {
            this->len = 0;
            delete[] this->values;
        }

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