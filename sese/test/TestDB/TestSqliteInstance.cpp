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
#include <sese/util/Initializer.h>
#include <gtest/gtest.h>
#include <cinttypes>

#include "Config.h"

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

TEST(TestSqliteDriverInstance, Error) {
    auto instance = DriverManager::getInstance(DatabaseType::SQLITE, SQLITE_CONNECTION_STRING);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result_error = instance->executeQuery("select * from tb_query_error where id = 1;");
    ASSERT_EQ(nullptr, result_error);

    ASSERT_NE(nullptr, instance->getLastErrorMessage());

    //stmtError
    auto result_stmt_error = instance->createStatement("select * from tb_stmt_query_error where id = 1;");
    ASSERT_EQ(nullptr, result_stmt_error);

    ASSERT_NE(nullptr, instance->getLastErrorMessage());
}

TEST(TestSqliteDriverInstance, QueryData) {
    auto instance = DriverManager::getInstance(DatabaseType::SQLITE, SQLITE_CONNECTION_STRING);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_query;");
    ASSERT_NE(nullptr, result);
    ASSERT_EQ(2, result->getColumns());
    if (result->next()) {
        ASSERT_EQ(1, result->getInteger(0));
        std::string_view strings = "foo";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    if (result->next()) {
        ASSERT_EQ(2, result->getInteger(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    ASSERT_EQ(false, result->next());

    result->reset();

    if (result->next()) {
        ASSERT_EQ(1, result->getInteger(0));
        std::string_view strings = "foo";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    if (result->next()) {
        ASSERT_EQ(2, result->getInteger(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    ASSERT_EQ(false, result->next());
}

TEST(TestSqliteDriverInstance, UpdateData) {
    auto instance = DriverManager::getInstance(DatabaseType::SQLITE, SQLITE_CONNECTION_STRING);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto count = instance->executeUpdate(R"(update tb_update set name = "mike" where id = 1;)");
    ASSERT_NE(-1, count) << instance->getLastErrorMessage();

    auto result = instance->executeQuery("select * from tb_update where id = 1;");
    ASSERT_NE(nullptr, result);
    if (result->next()) {
        ASSERT_EQ(1, result->getInteger(0));
        std::string_view strings = "mike";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    ASSERT_EQ(false, result->next());
}

TEST(TestSqliteDriverInstance, DeleteData) {
    auto instance = DriverManager::getInstance(DatabaseType::SQLITE, SQLITE_CONNECTION_STRING);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto count = instance->executeUpdate("delete from tb_delete where id = 1;");
    ASSERT_NE(-1, count);

    auto result = instance->executeQuery("select * from tb_delete;");
    ASSERT_NE(nullptr, result);
    if (result->next()) {
        ASSERT_EQ(2, result->getInteger(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    ASSERT_EQ(false, result->next());
}

TEST(TestSqliteDriverInstance, InsertData) {
    auto instance = DriverManager::getInstance(DatabaseType::SQLITE, SQLITE_CONNECTION_STRING);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto count = instance->executeUpdate("insert into tb_insert (id, name) values (3, 'mike');");
    ASSERT_NE(-1, count);

    auto result = instance->executeQuery("select * from tb_insert;");
    ASSERT_NE(nullptr, result);
    if (result->next()) {
        ASSERT_EQ(1, result->getInteger(0));
        std::string_view strings = "foo";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    if (result->next()) {
        ASSERT_EQ(2, result->getInteger(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    if (result->next()) {
        ASSERT_EQ(3, result->getInteger(0));
        std::string_view strings = "mike";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    ASSERT_EQ(false, result->next());
}

TEST(TestSqliteDriverInstance, DateTime) {
    auto instance = DriverManager::getInstance(DatabaseType::SQLITE, SQLITE_CONNECTION_STRING);
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
}

TEST(TestSqliteDriverInstance, isNull) {
    auto instance = DriverManager::getInstance(DatabaseType::SQLITE, SQLITE_CONNECTION_STRING);
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