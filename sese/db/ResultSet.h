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

/// \file ResultSet.h
/// \brief Query result set
/// \version 0.1
/// \author kaoru

#pragma once
#include <sese/db/Config.h>

#include <string>
#include <cstring>
#include <sese/util/DateTime.h>
#include <optional>

namespace sese::db {

    /// \brief Query result set
    class  ResultSet {
    public:
        using Ptr = std::unique_ptr<ResultSet>;

        virtual ~ResultSet() noexcept = default;
        /// \brief Locate the first query record
        virtual void reset() noexcept = 0;
        /// \brief Move to the next record
        /// \return Whether there is a next record
        [[nodiscard]] virtual bool next() noexcept = 0;
        /// \brief Get the number of columns in the record
        /// \return Number of columns in the result set record
        [[nodiscard]] virtual size_t getColumns() const noexcept = 0;

        /// \brief Get an integer value from the current record
        /// \param index Index
        /// \return Integer value
        [[nodiscard]] virtual int32_t getInteger(size_t index) const noexcept = 0;
        /// \brief Get a long integer value from the current record
        /// \param index Index
        /// \return Long integer value
        [[nodiscard]] virtual int64_t getLong(size_t index) const noexcept = 0;
        /// \brief Get a string from the current record
        /// \param index Index
        /// \return String
        [[nodiscard]] virtual std::string_view getString(size_t index) const noexcept = 0;
        /// \brief Get a double-precision floating-point value from the current record
        /// \param index Index
        /// \return Double-precision floating-point value
        [[nodiscard]] virtual double getDouble(size_t index) const noexcept = 0;
        /// \brief Get a single-precision floating-point value from the current record
        /// \param index Index
        /// \return Single-precision floating-point value
        [[nodiscard]] virtual float getFloat(size_t index) const noexcept = 0;
        /// \brief Get a time data structure from the current record
        /// \param index Index
        /// \return Time data structure
        [[nodiscard]] virtual std::optional<sese::DateTime> getDateTime(size_t index) const noexcept = 0;
        /// \brief Determine if the returned result is null
        /// \param index Index
        /// \return Whether it is null
        [[nodiscard]] virtual bool isNull(size_t index) const noexcept = 0;
    };
}