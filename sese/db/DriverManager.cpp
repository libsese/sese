#include <sese/db/DriverManager.h>
#include <sese/db/Util.h>

#ifdef HAS_MARIADB
#include <sese/internal/db/impl/maria/MariaDriverInstanceImpl.h>
#endif

#ifdef HAS_SQLITE
#include <sese/internal/db/impl/sqlite/SqliteDriverInstanceImpl.h>
#endif

#ifdef HAS_POSTGRES
#include <sese/internal/db/impl/pgsql/PostgresDriverInstanceImpl.h>
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
                // 缺少必要字段
                return nullptr;
            }

            MYSQL *conn = mysql_init(nullptr);
            if (conn == nullptr) {
                // 此处通常是不可恢复错误触发，例如内存不足
                return nullptr;
            }

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
                // 缺少必要字段
                return nullptr;
            }
            const char *keywords[] = {"host", "user", "password", "dbname", "port", nullptr};
            const char *values[] = {
                    host_iterator->second.c_str(),
                    user_iterator->second.c_str(),
                    pwd_iterator->second.c_str(),
                    db_iterator->second.c_str(),
                    port_iterator->second.c_str(),
                    nullptr};

            PGconn *conn = PQconnectdbParams(keywords, values, 0);
            if (conn == nullptr) {
                return nullptr;
            }
            return std::make_unique<impl::PostgresDriverInstanceImpl>(conn);
        }
#endif
        default:
            return nullptr;
    }
}