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

#include <sese/db/DriverManager.h>
#include <sese/db/Util.h>

#ifdef HAS_MARIADB
#include <sese/internal/db/maria/MariaDriverInstanceImpl.h>
#endif

#ifdef HAS_SQLITE
#include <sese/internal/db/sqlite/SqliteDriverInstanceImpl.h>
#endif

#ifdef HAS_POSTGRES
#include <sese/internal/db/pgsql/PostgresDriverInstanceImpl.h>
#endif

using namespace sese::db;

DriverInstance::Ptr DriverManager::getInstance(sese::db::DatabaseType type, const char *connection_string) noexcept {
    switch (type) {
#ifdef HAS_MARIADB
        case DatabaseType::MY_SQL:
        case DatabaseType::MARIA: {
            auto config = tokenize(connection_string);

            auto host_iterator = config.find("host");
            auto user_iterator = config.find("user");
            auto pwd_iterator = config.find("pwd");
            auto db_iterator = config.find("db");
            auto port_iterator = config.find("port");
            if (host_iterator == config.end() ||
                user_iterator == config.end() ||
                pwd_iterator == config.end() ||
                db_iterator == config.end() ||
                port_iterator == config.end()) {
                // Required fields are missing
                return nullptr;
            }

            MYSQL *conn = mysql_init(nullptr);
            // This is usually where an unrecoverable error is triggered, such as out of memory
            if (conn == nullptr) return nullptr; // GCOVR_EXCL_LINE

            mysql_real_connect(
                    conn,
                    host_iterator->second.c_str(),
                    user_iterator->second.c_str(),
                    pwd_iterator->second.c_str(),
                    db_iterator->second.c_str(),
                    std::stoi(port_iterator->second),
                    nullptr,
                    0
            );

            return std::make_unique<impl::MariaDriverInstanceImpl>(conn);
        }
#endif
#ifdef HAS_SQLITE
        case DatabaseType::SQLITE: {
            sqlite3 *conn;
            sqlite3_open_v2(connection_string, &conn, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
            return std::make_unique<impl::SqliteDriverInstanceImpl>(conn);
        }
#endif
#ifdef HAS_POSTGRES
        case DatabaseType::POSTGRES: {
            auto config = tokenize(connection_string);

            auto host_iterator = config.find("host");
            auto user_iterator = config.find("user");
            auto pwd_iterator = config.find("pwd");
            auto db_iterator = config.find("db");
            auto port_iterator = config.find("port");
            if (host_iterator == config.end() ||
                user_iterator == config.end() ||
                pwd_iterator == config.end() ||
                db_iterator == config.end() ||
                port_iterator == config.end()) {
                // Required fields are missing
                return nullptr;
            }
            const char *keywords[] = {"host", "user", "password", "dbname", "port", nullptr};
            const char *values[] = {
                    host_iterator->second.c_str(),
                    user_iterator->second.c_str(),
                    pwd_iterator->second.c_str(),
                    db_iterator->second.c_str(),
                    port_iterator->second.c_str(),
                    nullptr
            };

            PGconn *conn = PQconnectdbParams(keywords, values, 0);
            if (conn == nullptr) return nullptr; // GCOVR_EXCL_LINE
            return std::make_unique<impl::PostgresDriverInstanceImpl>(conn);
        }
#endif
        // GCOVR_EXCL_START
        default:
            return nullptr;
            // GCOVR_EXCL_STOP
    }
}