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

TEST(TestPostgresTransaction, Commit) {
    auto instance = DriverManager::getInstance(
            DatabaseType::POSTGRES,
            PSQL_CONNECTION_STRING
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    ASSERT_EQ(true, instance->begin());

    auto count = instance->executeUpdate("insert into tb_commit (id, name) values (3, 'mike')");
    ASSERT_NE(-1, count);

    ASSERT_EQ(true, instance->commit());

    auto results = instance->executeQuery("select * from tb_commit;");
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

TEST(TestPostgresTransaction, RollBack) {
    auto instance = DriverManager::getInstance(
            DatabaseType::POSTGRES,
            PSQL_CONNECTION_STRING
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    ASSERT_EQ(true, instance->begin());

    auto count = instance->executeUpdate("insert into tb_rollBack (id, name) values (3, 'mike');");
    ASSERT_NE(-1, count);

    ASSERT_EQ(true, instance->rollback());

    auto results = instance->executeQuery("select * from tb_rollBack;");
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
    ASSERT_EQ(false, results->next());
}

TEST(TestPostgresTransaction, Begin) {
    auto instance = DriverManager::getInstance(
            DatabaseType::POSTGRES,
            PSQL_CONNECTION_STRING
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    // 事务的回滚操作
    ASSERT_EQ(true, instance->begin());

    auto count = instance->executeUpdate("insert into tb_begin (id, name) values (3, 'mike');");
    ASSERT_NE(-1, count);

    ASSERT_EQ(true, instance->rollback());

    auto results = instance->executeQuery("select * from tb_begin;");
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
    ASSERT_EQ(false, results->next());

    // 事务的提交操作
    ASSERT_EQ(true, instance->begin());

    auto count1 = instance->executeUpdate("insert into tb_begin (id, name) values (3, 'mike');");
    ASSERT_NE(-1, count1);

    ASSERT_EQ(true, instance->commit());

    auto results1 = instance->executeQuery("select * from tb_begin;");
    ASSERT_NE(nullptr, results1);
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
    if (results1->next()) {
        ASSERT_EQ(3, results1->getInteger(0));
        std::string_view strings = "mike";
        ASSERT_EQ(strings, results1->getString(1).data());
    }
    ASSERT_EQ(false, results1->next());
}

TEST(TestPostgresTransaction, UseLess) {
    auto instance = DriverManager::getInstance(
            DatabaseType::POSTGRES,
            PSQL_CONNECTION_STRING
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    // auto stmt = instance->createStatement("select * from tb_query_error;");
    // auto results1 = stmt->executeQuery();

    // auto stmt2 = instance->createStatement("update tb_update set name = 'mike' where id = 1;");
    // auto results2 = stmt->executeUpdate();

    auto a = instance->setAutoCommit(true);

    bool b1;
    auto b = instance->getAutoCommit(b1);

    int64_t c1;
    auto c = instance->getInsertId(c1);
}