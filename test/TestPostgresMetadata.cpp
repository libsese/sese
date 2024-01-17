#include <gtest/gtest.h>
#include <sese/db/DriverManager.h>

class TestPostgresMetadata : public testing::Test {
protected:
    static sese::db::DriverInstance::Ptr instance;

    static void SetUpTestSuite() {
        instance = sese::db::DriverManager::getInstance(sese::db::DatabaseType::Postgres, "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;");
        ASSERT_NE(nullptr, instance);
    }
};

sese::db::DriverInstance::Ptr TestPostgresMetadata::instance;