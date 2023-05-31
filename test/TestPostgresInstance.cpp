#include <sese/db/DriverManager.h>
#include <gtest/gtest.h>

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

TEST(TestDriverInstance, TestPostgreInstance) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=qiuchenli;db=db_test;port=5432;"
    );
    ASSERT_NE(nullptr, instance);
}