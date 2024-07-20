/// \file Range.h
/// \author kaoru
/// \brief 区间类
/// \date 2023年9月7日
/// \version 0.1

#pragma once

#include <sese/Config.h>

namespace sese {

/// \brief 区间类
/// \tparam TYPE 值类型
template<class TYPE>
class Range {
public:
    /// 该构造函数只能用于正数且递增的区间
    /// \param count 区间大小
    /// \note [1, count]
    explicit Range(TYPE count) {
        this->beginNumber = 1;
        this->endNumber = count;
    }

    /// 该构造函数用于任意区间无关递增或递减的区间
    /// \param begin 起始
    /// \param end 结束
    /// \note [begin, end]
    Range(TYPE begin, TYPE end) {
        if (begin > end) {
            std::swap(begin, end);
        }

        this->beginNumber = begin;
        this->endNumber = end;
    }

    /// 迭代器
    class Iterator {
    public:
        using difference_type = int64_t;
        using value_type = TYPE;
        using pointer = const TYPE *;
        using reference = const TYPE &;
        using iterator_category = std::forward_iterator_tag;

        explicit Iterator(TYPE cur_number) : curNumber(cur_number) {}

        Iterator &operator++() {
            this->curNumber += 1;
            return *this;
        }

        Iterator operator++(int) { // NOLINT
            Iterator iterator = *this;
            iterator.curNumber += 1;
            return iterator;
        }

        bool operator==(Iterator iterator) const {
            return this->curNumber == iterator.curNumber;
        }

        bool operator!=(Iterator iterator) const {
            return this->curNumber != iterator.curNumber;
        }

        TYPE operator*() const {
            return this->curNumber;
        }

    private:
        TYPE curNumber = 0;
    };

    /// 反向迭代器
    class ReverseIterator {
    public:
        using difference_type = int64_t;
        using value_type = TYPE;
        using pointer = const TYPE *;
        using reference = const TYPE &;
        using iterator_category = std::forward_iterator_tag;

        explicit ReverseIterator(TYPE cur_number) : curNumber(cur_number) {}

        ReverseIterator &operator++() {
            this->curNumber -= 1;
            return *this;
        }

        ReverseIterator operator++(int) { // NOLINT
            Iterator iterator = *this;
            iterator.curNumber -= 1;
            return iterator;
        }

        bool operator==(ReverseIterator iterator) const {
            return this->curNumber == iterator.curNumber;
        }

        bool operator!=(ReverseIterator iterator) const {
            return this->curNumber != iterator.curNumber;
        }

        TYPE operator*() const {
            return this->curNumber;
        }

    private:
        TYPE curNumber = 0;
    };

    Iterator begin() {
        return Iterator(beginNumber);
    }

    Iterator end() {
        return Iterator(endNumber + 1);
    }

    ReverseIterator rbegin() {
        return ReverseIterator(endNumber);
    }

    ReverseIterator rend() {
        return ReverseIterator(beginNumber - 1);
    }

    /// 判断一个值是否处于区间内
    /// \param num 欲判断的值
    /// \return 结果
    bool exist(const TYPE &num) const {
        return this->beginNumber <= num && this->endNumber >= num;
    }

private:
    TYPE beginNumber = 0;
    TYPE endNumber = 0;
};
} // namespace sese