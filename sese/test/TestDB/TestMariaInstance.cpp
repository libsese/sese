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
#include <gtest/gtest.h>
#include <cinttypes>

#include "Config.h"

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

TEST(TestMariaDriverInstance, InstanceError) {
    auto instance = DriverManager::getInstance(
            DatabaseType::MARIA,
            "host=127.0.0.1;user=root;pwd=libsese;port=18806;"
    );
    ASSERT_EQ(nullptr, instance);
}

TEST(TestMariaDriverInstance, MissingArgs) {
    EXPECT_EQ(nullptr, DriverManager::getInstance(DatabaseType::MY_SQL, "host=x;port=x;user=x;pwd=x;"));
    EXPECT_EQ(nullptr, DriverManager::getInstance(DatabaseType::MY_SQL, "host=x;port=x;user=x;db=x;"));
    EXPECT_EQ(nullptr, DriverManager::getInstance(DatabaseType::MY_SQL, "host=x;port=x;db=x;pwd=x;"));
    EXPECT_EQ(nullptr, DriverManager::getInstance(DatabaseType::MY_SQL, "host=x;db=x;user=x;pwd=x;"));
    EXPECT_EQ(nullptr, DriverManager::getInstance(DatabaseType::MY_SQL, "db=x;port=x;user=x;pwd=x;"));
}

TEST(TestMariaDriverInstance, QueryData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::MARIA,
            MYSQL_CONNECTION_STRING
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    // error
    auto result = instance->executeQuery("select * from tb_query_error where id = 1");
    ASSERT_EQ(nullptr, result);
    ASSERT_NE(nullptr, instance->getLastErrorMessage());

    auto result1 = instance->executeQuery("select * from tb_query;");
    ASSERT_NE(nullptr, result1);
    ASSERT_EQ(2, result1->getColumns());

    if (result1->next()) {
        ASSERT_EQ(1, result1->getInteger(0));
        std::string_view strings = "foo";
        ASSERT_EQ(strings, result1->getString(1).data());
    }
    if (result1->next()) {
        ASSERT_EQ(2, result1->getInteger(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, result1->getString(1).data());
    }
    ASSERT_EQ(false, result1->next());

    result1->reset();

    if (result1->next()) {
        ASSERT_EQ(1, result1->getInteger(0));
        std::string_view strings = "foo";
        ASSERT_EQ(strings, result1->getString(1).data());
    }
    if (result1->next()) {
        ASSERT_EQ(2, result1->getInteger(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, result1->getString(1).data());
    }
    ASSERT_EQ(false, result1->next());
}

TEST(TestMariaDriverInstance, Modifyata) {
    auto instance = DriverManager::getInstance(
            DatabaseType::MARIA,
            MYSQL_CONNECTION_STRING
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto count = instance->executeUpdate("update tb_update set name = 'bar' where id = 1;");
    ASSERT_NE(-1, count);

    //error
    auto count1 = instance->executeUpdate("update tb_update_error set name = 'bar' where id = 1;");
    ASSERT_EQ(-1, count1);
    ASSERT_NE(nullptr, instance->getLastErrorMessage());


    auto result = instance->executeQuery("select * from tb_update where id = 1;");
    ASSERT_NE(nullptr, result);
    if (result->next()) {
        ASSERT_EQ(1, result->getInteger(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    ASSERT_EQ(false, result->next());
}

TEST(TestMariaDriverInstance, InsertData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::MARIA,
            MYSQL_CONNECTION_STRING
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto count = instance->executeUpdate("insert into tb_insert (id, name, setDouble, setFloat, setLong) values (3, 'mike', 3, 3, 3);");
    ASSERT_NE(-1, count);

    // error
    auto count1 = instance->executeUpdate("insert into tb_insert_error (id, name) values (3, 'mike');");
    ASSERT_EQ(-1, count1);
    ASSERT_NE(nullptr, instance->getLastErrorMessage());

    auto result = instance->executeQuery("select * from tb_insert;");
    ASSERT_NE(nullptr, result);
    if (result->next()) {
        ASSERT_EQ(1, result->getInteger(0));
        ASSERT_EQ(1, result->getDouble(2));
        ASSERT_EQ(1, result->getFloat(3));
        ASSERT_EQ(1, result->getLong(4));
        std::string_view strings = "foo";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    if (result->next()) {
        ASSERT_EQ(2, result->getInteger(0));
        ASSERT_EQ(2, result->getInteger(0));
        ASSERT_EQ(2, result->getDouble(2));
        ASSERT_EQ(2, result->getFloat(3));
        ASSERT_EQ(2, result->getLong(4));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    if (result->next()) {
        ASSERT_EQ(3, result->getInteger(0));
        ASSERT_EQ(3, result->getInteger(0));
        ASSERT_EQ(3, result->getDouble(2));
        ASSERT_EQ(3, result->getFloat(3));
        ASSERT_EQ(3, result->getLong(4));
        std::string_view strings = "mike";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    ASSERT_EQ(false, result->next());
}

TEST(TestMariaDriverInstance, DeleteData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::MARIA,
            MYSQL_CONNECTION_STRING
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto count = instance->executeUpdate("delete from tb_delete where id = 1;");
    ASSERT_NE(-1, count);

    // error
    auto count1 = instance->executeUpdate("delete tb_delete_error set name = 'bar' where id = 1;");
    ASSERT_EQ(-1, count1);
    ASSERT_NE(nullptr, instance->getLastErrorMessage());

    auto result = instance->executeQuery("select * from tb_delete;");
    ASSERT_NE(nullptr, result);
    if (result->next()) {
        ASSERT_EQ(2, result->getInteger(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    ASSERT_EQ(false, result->next());
}

TEST(TestMariaDriverInstance, DateTime) {
    auto instance = DriverManager::getInstance(
            DatabaseType::MARIA,
            MYSQL_CONNECTION_STRING
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_dateTime;");
    ASSERT_NE(nullptr, result);

    if (result->next()) {
        ASSERT_EQ(1, result->getInteger(0));
        ASSERT_EQ(1679142600000000, result->getDateTime(1).value().getTimestamp());
    }
    if (result->next()) {
        ASSERT_EQ(2, result->getInteger(0));
        ASSERT_EQ(1679142600000000, result->getDateTime(1).value().getTimestamp());
    }
    ASSERT_EQ(false, result->next());
}

TEST(TestMariaDriverInstance, isNull) {
    auto instance = DriverManager::getInstance(
            DatabaseType::MARIA,
            MYSQL_CONNECTION_STRING
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

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