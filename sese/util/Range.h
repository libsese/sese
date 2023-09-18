/// \file Range.h
/// \author kaoru
/// \brief 区间类
/// \date 2023年9月7日
/// \version 0.1

#pragma once

#include <sese/Config.h>

namespace sese {

    /// \brief 区间类
    /// \tparam Type 值类型
    template<class Type>
    class Range {
    public:
        /// 该构造函数只能用于正数且递增的区间
        /// \param count 区间大小
        /// \note [1, count]
        explicit Range(Type count) {
            this->beginNumber = 1;
            this->endNumber = count;
        }

        /// 该构造函数用于任意区间无关递增或递减的区间
        /// \param begin 起始
        /// \param end 结束
        /// \note [begin, end]
        Range(Type begin, Type end) {
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
            using value_type = Type;
            using pointer = const Type *;
            using reference = const Type &;
            using iterator_category = std::forward_iterator_tag;

            explicit Iterator(Type curNumber) : curNumber(curNumber) {}

            Iterator &operator++() {
                this->curNumber += 1;
                return *this;
            }

            Iterator operator++(int) {// NOLINT
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

            Type operator*() const {
                return this->curNumber;
            }

        private:
            Type curNumber = 0;
        };

        /// 反向迭代器
        class ReverseIterator {
        public:
            using difference_type = int64_t;
            using value_type = Type;
            using pointer = const Type *;
            using reference = const Type &;
            using iterator_category = std::forward_iterator_tag;

            explicit ReverseIterator(Type curNumber) : curNumber(curNumber) {}

            ReverseIterator &operator++() {
                this->curNumber -= 1;
                return *this;
            }

            ReverseIterator operator++(int) {// NOLINT
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

            Type operator*() const {
                return this->curNumber;
            }

        private:
            Type curNumber = 0;
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
        bool exist(const Type &num) const {
            return this->beginNumber <= num && this->endNumber >= num;
        }

    private:
        Type beginNumber = 0;
        Type endNumber = 0;
    };
}// namespace sese