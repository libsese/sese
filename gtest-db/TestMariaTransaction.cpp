#include <sese/db/DriverManager.h>
#include <gtest/gtest.h>
#include <cinttypes>

#include "Config.h"

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

TEST(TestMariaTransaction, GetAutoCommit) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            MYSQL_CONNECTION_STRING

    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    bool status;
    ASSERT_EQ(true, instance->getAutoCommit(status));
    ASSERT_EQ(1, status);
}

TEST(TestMariaTransaction, SetAutoCommit) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            MYSQL_CONNECTION_STRING

    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    bool status;
    ASSERT_EQ(true, instance->getAutoCommit(status));
    ASSERT_EQ(1, status);
    ASSERT_EQ(true, instance->setAutoCommit(false));

    ASSERT_EQ(true, instance->getAutoCommit(status));
    ASSERT_EQ(0, status);

    ASSERT_EQ(true, instance->setAutoCommit(true));
    ASSERT_EQ(true, instance->getAutoCommit(status));
    ASSERT_EQ(1, status);
}

TEST(TestMariaTransaction, Commit) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            MYSQL_CONNECTION_STRING

    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    bool status;
    ASSERT_EQ(true, instance->setAutoCommit(false));
    ASSERT_EQ(true, instance->getAutoCommit(status));
    ASSERT_EQ(0, status);

    auto count = instance->executeUpdate("insert into tb_commit (id, name) values (3, 'mike')");
    ASSERT_NE(0, count);

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

TEST(TestMariaTransaction, RollBack) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            MYSQL_CONNECTION_STRING

    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    bool status;
    ASSERT_EQ(true, instance->setAutoCommit(false));
    ASSERT_EQ(true, instance->getAutoCommit(status));
    ASSERT_EQ(0, status);

    auto count = instance->executeUpdate("insert into tb_rollBack (id, name) values (3, 'mike');");
    ASSERT_NE(0, count);

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

TEST(TestMariaTransaction, Begin) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            MYSQL_CONNECTION_STRING

    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    // 事务的回滚操作
    ASSERT_EQ(true, instance->begin());

    auto count = instance->executeUpdate("insert into tb_begin (id, name) values (3, 'mike');");
    ASSERT_NE(0, count);

    ASSERT_EQ(true, instance->rollback());

    auto results = instance->executeQuery("select * from tb_begin;");
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

    // 事务的提交操作
    ASSERT_EQ(true, instance->begin());

    auto count1 = instance->executeUpdate("insert into tb_begin (id, name) values (3, 'mike');");
    ASSERT_NE(0, count1);

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

TEST(TestMariaTransaction, GetInserId) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            MYSQL_CONNECTION_STRING

    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto count = instance->executeUpdate("insert into tb_getInsertId (name) values ('mike');");
    ASSERT_NE(0, count);

    int64_t id = 0;
    ASSERT_EQ(true, instance->getInsertId(id));
    ASSERT_EQ(3, id);

    auto results = instance->executeQuery("select * from tb_getInsertId;");
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