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

/// \file PreparedStatement.h
/// \author kaoru
/// \brief Prepared statement object
/// \version 0.2

#pragma once

#include <sese/db/ResultSet.h>
#include <sese/db/Metadata.h>
#include <sese/text/DateTimeFormatter.h>
#include <sese/util/ErrorCode.h>

namespace sese::db {

    /// \brief Prepared statement object
    class  PreparedStatement {
    public:
        using Ptr = std::unique_ptr<PreparedStatement>;

        virtual ~PreparedStatement() noexcept = default;

        /// \brief Execute query
        /// \return Query result set
        /// \retval nullptr Query failed
        virtual ResultSet::Ptr executeQuery() noexcept = 0;
        /// \brief Execute update
        /// \return Number of rows affected
        /// \retval -1 Update failed
        virtual int64_t executeUpdate() noexcept = 0;

        /// \brief Set double-precision floating-point value
        /// \param index Index
        /// \param value Value
        /// \return Whether the setting was successful
        virtual bool setDouble(uint32_t index, const double &value) noexcept = 0;
        /// \brief Set single-precision floating-point value
        /// \param index Index
        /// \param value Value
        /// \return Whether the setting was successful
        virtual bool setFloat(uint32_t index, const float &value) noexcept = 0;
        /// \brief Set integer value
        /// \param index Index
        /// \param value Value
        /// \return Whether the setting was successful
        virtual bool setInteger(uint32_t index, const int32_t &value) noexcept = 0;
        /// \brief Set long integer value
        /// \param index Index
        /// \param value Value
        /// \return Whether the setting was successful
        virtual bool setLong(uint32_t index, const int64_t &value) noexcept = 0;
        /// \brief Set text value
        /// \param index Index
        /// \param value Value
        /// \return Whether the setting was successful
        virtual bool setText(uint32_t index, const char *value) noexcept = 0;
        /// \brief Set to NULL
        /// \param index Index
        /// \return Whether the setting was successful
        virtual bool setNull(uint32_t index) noexcept = 0;
        /// \brief Set to dateTime
        /// \param index Index
        /// \param value Date
        /// \return Whether the setting was successful
        virtual bool setDateTime(uint32_t index, const sese::DateTime &value) noexcept = 0;
        /// \brief Get result set column type
        /// \param index Index value
        /// \param type Column type
        /// \return Whether the retrieval was successful
        virtual bool getColumnType(uint32_t index, MetadataType &type) noexcept = 0;
        /// \brief Get result set column size. This interface is usually only effective when the type is binary or string, and the unit is bytes.
        /// \param index Index value
        /// \return Returns -1 if failed
        virtual int64_t getColumnSize(uint32_t index) noexcept = 0;

        /// \brief Get the error code for the corresponding driver of the instance
        /// \return Error code
        [[nodiscard]] virtual int getLastError() const noexcept = 0;

        /// \brief Get the error message for the corresponding driver of the instance. Need to check for null.
        /// \retval nullptr Failed to get error message
        /// \return Error message
        [[nodiscard]] virtual const char *getLastErrorMessage() const noexcept = 0;

        ErrorCode getErrorCode() const {
            return {getLastError(), getLastErrorMessage()};
        }
    };
}// namespace sese::db