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

/// \file PostgresResultSetImpl.h
/// \brief PSQL result set implementation
/// \author kaoru, DreammerQcl
/// \date April 10, 2024

#pragma once

#include <sese/db/ResultSet.h>
#include <libpq-fe.h>

namespace sese::db::impl {
    /// \brief PSQL result set implementation
    class  PostgresResultSetImpl : public ResultSet {
    public:
        explicit PostgresResultSetImpl(PGresult *res) noexcept;
        ~PostgresResultSetImpl() noexcept override;

        void reset() noexcept override;
        [[nodiscard]] bool next() noexcept override;
        [[nodiscard]] bool isNull(size_t index) const noexcept override;
        [[nodiscard]] size_t getColumns() const noexcept override;

        [[nodiscard]] int32_t getInteger(size_t index) const noexcept override;
        [[nodiscard]] int64_t getLong(size_t index) const noexcept override;
        [[nodiscard]] std::string_view getString(size_t index) const noexcept override;
        [[nodiscard]] double getDouble(size_t index) const noexcept override;
        [[nodiscard]] float getFloat(size_t index) const noexcept override;
        [[nodiscard]] std::optional<sese::DateTime> getDateTime(size_t index) const noexcept override;
    protected:
        PGresult *res;
        int row;
        int totalRow;
    };
}// namespace sese::db::impl