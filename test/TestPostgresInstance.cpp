#include <sese/db/DriverManager.h>
#include <gtest/gtest.h>

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

// query
TEST(TestDriverInstance, TestQueryData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;password=libsese;dbname=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    auto results = instance->executeQuery("select * from tb_query;");
    ASSERT_NE(nullptr, results);
    while (results->next()) {
        printf("id = %d, name = %s\n", (int) results->getInteger(0), results->getString(1).data());
    }
}

// update
//TEST(TestDriverInstance, TestUpdateData) {
//    auto instance = DriverManager::getInstance(
//            DatabaseType::Postgres,
//            "host=127.0.0.1;user=postgres;password=qiuchenli;dbname=db_test;port=5432;"
//    );
//    ASSERT_NE(nullptr, instance);
//
//    auto results = instance->executeQuery("select * from tb_update;");
//    while (results->next()) {
//        printf("id = %d, name = %s\n", (int) results->getInteger(0), results->getString(1).data());
//    }
//
//    auto count = instance->executeUpdate("update tb_update set name = 'mike' where id = 1;");
//    ASSERT_NE(0, count);
//
//    auto results1 = instance->executeQuery("select * from tb_update;");
//    while (results1->next()) {
//        printf("id = %d, name = %s\n", (int) results1->getInteger(0), results1->getString(1).data());
//    }
//}