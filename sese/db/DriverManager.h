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

/// \file DriverManager.h
/// \author kaoru
/// \brief Database driver instance manager
/// \version 0.1

#pragma once
#include <sese/db/DriverInstance.h>

namespace sese::db {
    /// \brief Database type enumeration
    enum class DatabaseType {
        MY_SQL,
        MARIA,
        SQLITE,
        POSTGRES
    };

    /// \brief Database driver instance manager
    class  DriverManager {
    public:
        /// \brief Create a database driver instance
        /// \param type Database type
        /// \param conn_string Connection string
        /// \return Database driver instance
        /// \retval nullptr Failed to create a database driver instance, which is usually a non-recoverable error but occurs rarely
        static DriverInstance::Ptr getInstance(DatabaseType type, const char *conn_string) noexcept;
    };
}// namespace sese::db