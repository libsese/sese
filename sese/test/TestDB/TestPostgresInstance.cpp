#include <sese/db/DriverManager.h>
#include <sese/util/Initializer.h>
#include <gtest/gtest.h>
#include <cinttypes>

#include "Config.h"

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

TEST(TestPostgresDriverInstance, InstanceError) {
    auto instance = DriverManager::getInstance(
            DatabaseType::POSTGRES,
            "host=127.0.0.1;user=POSTGRES;pwd=error;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_NE(instance->getLastError(), 0);
    ASSERT_NE(nullptr, instance->getLastErrorMessage());

    auto results = instance->executeQuery("select * from tb_query;");

    auto results1 = instance->executeUpdate("update tb_update set name = 'mike' where id = 1;");

    auto stmt = instance->createStatement("select * from tb_query;");
    // auto results2 = stmt->executeQuery();

    auto stmt2 = instance->createStatement("update tb_update set name = 'mike' where id = 1;");
    // auto results3 = stmt2->executeUpdate();

    auto results4 = instance->commit();

    auto results5 = instance->rollback();

    auto results6 = instance->begin();
}

TEST(TestPostgresDriverInstance, MissingArgs) {
    EXPECT_EQ(nullptr, DriverManager::getInstance(DatabaseType::POSTGRES, "host=x;port=x;user=x;pwd=x;"));
    EXPECT_EQ(nullptr, DriverManager::getInstance(DatabaseType::POSTGRES, "host=x;port=x;user=x;db=x;"));
    EXPECT_EQ(nullptr, DriverManager::getInstance(DatabaseType::POSTGRES, "host=x;port=x;db=x;pwd=x;"));
    EXPECT_EQ(nullptr, DriverManager::getInstance(DatabaseType::POSTGRES, "host=x;db=x;user=x;pwd=x;"));
    EXPECT_EQ(nullptr, DriverManager::getInstance(DatabaseType::POSTGRES, "db=x;port=x;user=x;pwd=x;"));
}

TEST(TestPostgresDriverInstance, QueryData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::POSTGRES,
            PSQL_CONNECTION_STRING
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    // error
    auto results = instance->executeQuery("select * from tb_query_error;");
    ASSERT_EQ(nullptr, results);
    ASSERT_NE(instance->getLastError(), 0);
    ASSERT_NE(nullptr, instance->getLastErrorMessage());

    auto results1 = instance->executeQuery("select * from tb_query;");
    ASSERT_NE(nullptr, results1);
    ASSERT_EQ(2, results1->getColumns());
    if (results1->next()) {
        ASSERT_EQ(1, results1->getInteger(0));
        std::string_view strings = "foo";
        ASSERT_EQ(strings, results1->getString(1).data());
    }
    if (results1->next()) {
        ASSERT_EQ(2, results1->getInteger(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, results1->getString(1).data());
    }
    ASSERT_EQ(false, results1->next());

    results1->reset();

    if (results1->next()) {
        ASSERT_EQ(1, results1->getInteger(0));
        std::string_view strings = "foo";
        ASSERT_EQ(strings, results1->getString(1).data());
    }
    if (results1->next()) {
        ASSERT_EQ(2, results1->getInteger(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, results1->getString(1).data());
    }
    ASSERT_EQ(false, results1->next());

    results1 = instance->executeQuery("select * from tb_query where id = 1;");
    ASSERT_NE(nullptr, results1);
    if (results1->next()) {
        ASSERT_EQ(1, results1->getInteger(0));
        std::string_view strings = "foo";
        ASSERT_EQ(strings, results1->getString(1).data());
    }
    ASSERT_EQ(false, results1->next());
}

TEST(TestPostgresDriverInstance, UpdateData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::POSTGRES,
            PSQL_CONNECTION_STRING
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    auto count = instance->executeUpdate("update tb_update set name = 'mike' where id = 1;");
    ASSERT_NE(-1, count);

    count = instance->executeUpdate("update tb_update set name = 'bar' where id = 2;");
    ASSERT_NE(-1, count);

    // error
    auto count1 = instance->executeUpdate("update tb_update_error set name = 'mike' where id = 1;");
    ASSERT_EQ(-1, count1);
    ASSERT_NE(instance->getLastError(), 0);
    ASSERT_NE(instance->getLastErrorMessage(), nullptr);

    auto results = instance->executeQuery("select * from tb_update;");
    ASSERT_NE(nullptr, results);
    if (results->next()) {
        ASSERT_EQ(1, results->getInteger(0));
        std::string_view strings = "mike";
        ASSERT_EQ(strings, results->getString(1).data());
    }
    if (results->next()) {
        ASSERT_EQ(2, results->getInteger(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, results->getString(1).data());
    }
    ASSERT_EQ(false, results->next());
}

TEST(TestPostgresDriverInstance, DeleteData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::POSTGRES,
            PSQL_CONNECTION_STRING
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    auto count = instance->executeUpdate("delete from tb_delete where id = 1;");
    ASSERT_NE(-1, count);

    auto results = instance->executeQuery("select * from tb_delete;");
    ASSERT_NE(nullptr, results);
    if (results->next()) {
        ASSERT_EQ(2, results->getInteger(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, results->getString(1).data());
    }
    ASSERT_EQ(false, results->next());
}

TEST(TestPostgresDriverInstance, InsertData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::POSTGRES,
            PSQL_CONNECTION_STRING
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    auto count = instance->executeUpdate("insert into tb_insert (id, name) values (3,'mike');");
    ASSERT_NE(-1, count);

    auto results = instance->executeQuery("select * from tb_insert;");
    ASSERT_NE(nullptr, results);
    if (results->next()) {
        ASSERT_EQ(1, results->getInteger(0));
        std::string_view strings = "foo";
        ASSERT_EQ(strings, results->getString(1).data());
    }
    if (results->next()) {
        ASSERT_EQ(2, results->getInteger(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, results->getString(1).data());
    }
    if (results->next()) {
        ASSERT_EQ(3, results->getInteger(0));
        std::string_view strings = "mike";
        ASSERT_EQ(strings, results->getString(1).data());
    }
    ASSERT_EQ(false, results->next());
}

TEST(TestPostgresDriverInstance, Datetime) {
    auto instance = DriverManager::getInstance(
            DatabaseType::POSTGRES,
            PSQL_CONNECTION_STRING
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    auto results = instance->executeQuery("select * from tb_dateTime;");
    ASSERT_NE(nullptr, results);
    if (results->next()) {
        ASSERT_EQ(1, results->getInteger(0));
        ASSERT_EQ(1679142600000000, results->getDateTime(1).value().getTimestamp());
    }
    if (results->next()) {
        ASSERT_EQ(2, results->getInteger(0));
        ASSERT_EQ(1679142600000000, results->getDateTime(1).value().getTimestamp());
    }
    ASSERT_EQ(false, results->next());
}

TEST(TestPostgresDriverInstance, isNull) {
    auto instance = DriverManager::getInstance(
            DatabaseType::POSTGRES,
            PSQL_CONNECTION_STRING
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    auto results = instance->executeQuery("select * from tb_isNull");
    ASSERT_NE(nullptr, results);

    ASSERT_TRUE(results->next());
    EXPECT_FALSE(results->isNull(0));
    EXPECT_FALSE(results->isNull(1));
    EXPECT_FALSE(results->isNull(2));
    EXPECT_FALSE(results->isNull(3));
    EXPECT_FALSE(results->isNull(4));
    EXPECT_FALSE(results->isNull(5));

    ASSERT_TRUE(results->next());
    EXPECT_TRUE(results->isNull(0));
    EXPECT_TRUE(results->isNull(1));
    EXPECT_TRUE(results->isNull(2));
    EXPECT_TRUE(results->isNull(3));
    EXPECT_TRUE(results->isNull(4));
    EXPECT_TRUE(results->isNull(5));
}