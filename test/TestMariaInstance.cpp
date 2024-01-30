#include <sese/db/DriverManager.h>
#include <gtest/gtest.h>
#include <cinttypes>

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

TEST(TestMariaDriverInstance, InstanceError) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;port=18806;"
    );
    ASSERT_EQ(nullptr, instance);
}

TEST(TestMariaDriverInstance, QueryData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    // error
    auto result = instance->executeQuery("select * from tb_query_error where id = 1");
    ASSERT_EQ(nullptr, result);
    printf("errorMsg: %s\n", instance->getLastErrorMessage());

    auto result1 = instance->executeQuery("select * from tb_query;");
    ASSERT_NE(nullptr, result1);
    printf("columns: %zu\n", result1->getColumns());
    while (result1->next()) {
        printf("result: id = %" PRId32 " name = %s\n", result1->getInteger(0), result1->getString(1).data());
    }

    result1->reset();

    while (result1->next()) {
        printf("result: id = %" PRId32 " name = %s\n", result1->getInteger(0), result1->getString(1).data());
    }
}

TEST(TestMariaDriverInstance, Modifyata) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_update where id = 1;");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("result: id = %" PRId32 " name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    auto count = instance->executeUpdate("update tb_update set name = 'bar' where id = 1;");
    ASSERT_NE(-1, count);

    //error
    auto count1 = instance->executeUpdate("update tb_update_error set name = 'bar' where id = 1;");
    ASSERT_EQ(-1, count1);
    printf("errorMsg: %s\n", instance->getLastErrorMessage());

    auto result1 = instance->executeQuery("select * from tb_update where id = 1;");
    ASSERT_NE(nullptr, result1);
    while (result1->next()) {
        printf("result1: id = %" PRId32 " name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

TEST(TestMariaDriverInstance, InsertData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_insert;");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("result: id = %" PRId32 " name = %s double = %lf setFloat = %f setLong = %" PRId64 "\n", result->getInteger(0), result->getString(1).data(), result->getDouble(2), result->getFloat(3), result->getLong(4));
    }

    auto count = instance->executeUpdate("insert into tb_insert (id, name, setDouble, setFloat, setLong) values (3, 'mike', 3, 3, 3);");
    ASSERT_NE(-1, count);

    // error
    auto count1 = instance->executeUpdate("insert into tb_insert_error (id, name) values (3, 'mike');");
    ASSERT_EQ(-1, count1);
    printf("errorMsg: %s\n", instance->getLastErrorMessage());

    auto result1 = instance->executeQuery("select * from tb_insert;");
    ASSERT_NE(nullptr, result1);
    while (result1->next()) {
        printf("result1: id = %" PRId32 " name = %s double = %lf setFloat = %f setLong = %" PRId64 "\n", result1->getInteger(0), result1->getString(1).data(), result1->getDouble(2), result1->getFloat(3), result1->getLong(4));
    }
}

TEST(TestMariaDriverInstance, DeleteData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_delete;");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("result: id = %" PRId32 " name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    auto count = instance->executeUpdate("delete from tb_delete where id = 1;");
    ASSERT_NE(-1, count);

    // error
    auto count1 = instance->executeUpdate("update tb_delete_error set name = 'bar' where id = 1;");
    ASSERT_EQ(-1, count1);
    printf("errorMsg: %s\n", instance->getLastErrorMessage());

    auto result1 = instance->executeQuery("select * from tb_delete;");
    ASSERT_NE(nullptr, result1);
    while (result1->next()) {
        printf("result1: id = %" PRId32 " name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

TEST(TestMariaStmt, dateTime) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_dateTime;");
    ASSERT_NE(nullptr, result);
    printf("columns: %zu\n", result->getColumns());
    while (result->next()) {
        printf("id = %" PRId32 " time = %" PRId64 "\n", result->getInteger(0), result->getDateTime(1).value().getTimestamp());
    }
}