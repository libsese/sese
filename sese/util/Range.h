// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// \file Range.h
/// \author kaoru
/// \brief Range class
/// \date September 7, 2023
/// \version 0.1

#pragma once

#include <sese/Config.h>

namespace sese {

/// \brief Range class
/// \tparam TYPE Value type
template<class TYPE>
class Range {
public:
    /// This constructor is only for positive and incrementing ranges
    /// \param count The size of the range
    /// \note [1, count]
    explicit Range(TYPE count) {
        this->beginNumber = 1;
        this->endNumber = count;
    }

    /// This constructor is for any range, regardless of whether it is incrementing or decrementing
    /// \param begin Start
    /// \param end End
    /// \note [begin, end]
    Range(TYPE begin, TYPE end) {
        if (begin > end) {
            std::swap(begin, end);
        }

        this->beginNumber = begin;
        this->endNumber = end;
    }

    /// Iterators
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

    /// Reverse Iterator
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

    /// Check if a value is within the range
    /// \param num The value to check
    /// \return The result
    bool exist(const TYPE &num) const {
        return this->beginNumber <= num && this->endNumber >= num;
    }

private:
    TYPE beginNumber = 0;
    TYPE endNumber = 0;
};
} // namespace sese