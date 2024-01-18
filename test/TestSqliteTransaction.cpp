#include <sese/db/DriverManager.h>
#include <sese/util/Initializer.h>
#include <gtest/gtest.h>
#include <cinttypes>

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

TEST(TestSqliteTransaction, GetAutoCommit) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    bool status;
    ASSERT_EQ(true, instance->getAutoCommit(status));
    printf("autoCommit = %d\n", status);
}

TEST(TestSqliteTransaction, Begin) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    // 事务的回滚操作
    auto results = instance->executeQuery("select * from tb_begin;");
    while (results->next()) {
        printf("id = %" PRId32 ", name = %s\n", results->getInteger(0), results->getString(1).data());
    }

    ASSERT_EQ(true, instance->begin());

    auto count = instance->executeUpdate("insert into tb_begin (id, name) values (3, 'mike');");
    ASSERT_NE(0, count);

    ASSERT_EQ(true, instance->rollback());

    auto results1 = instance->executeQuery("select * from tb_begin;");
    while (results1->next()) {
        printf("id = %" PRId32 ", name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }

    // 事务的提交操作
    auto results2 = instance->executeQuery("select * from tb_begin;");
    while (results2->next()) {
        printf("id = %" PRId32 ", name = %s\n", results2->getInteger(0), results2->getString(1).data());
    }

    ASSERT_EQ(true, instance->begin());

    auto count1 = instance->executeUpdate("insert into tb_begin (id, name) values (4, 'mike');");
    ASSERT_NE(0, count1);

    ASSERT_EQ(true, instance->commit());

    auto results3 = instance->executeQuery("select * from tb_begin;");
    while (results3->next()) {
        printf("id = %" PRId32 ", name = %s\n", results3->getInteger(0), results3->getString(1).data());
    }
}

TEST(TestSqliteTransaction, Commit) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto results = instance->executeQuery("select * from tb_commit;");
    while (results->next()) {
        printf("id = %" PRId32 ", name = %s\n", results->getInteger(0), results->getString(1).data());
    }

    bool status;
    ASSERT_EQ(true, instance->begin());
    ASSERT_EQ(true, instance->getAutoCommit(status));
    printf("autoCommit = %d\n", status);

    auto count = instance->executeUpdate("insert into tb_commit (id, name) values (3, 'mike')");
    ASSERT_NE(0, count);

    ASSERT_EQ(true, instance->commit());

    auto results1 = instance->executeQuery("select * from tb_commit;");
    while (results1->next()) {
        printf("id = %" PRId32 ", name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }
}

TEST(TestSqliteTransaction, RollBack) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto results = instance->executeQuery("select * from tb_rollBack;");
    while (results->next()) {
        printf("id = %" PRId32 ", name = %s\n", results->getInteger(0), results->getString(1).data());
    }

    bool status;
    ASSERT_EQ(true, instance->begin());
    ASSERT_EQ(true, instance->getAutoCommit(status));
    printf("autoCommit = %d\n", status);

    auto count = instance->executeUpdate("insert into tb_rollBack (id, name) values (3, 'mike');");
    ASSERT_NE(0, count);

    ASSERT_EQ(true, instance->rollback());

    auto results1 = instance->executeQuery("select * from tb_rollBack;");
    while (results1->next()) {
        printf("id = %" PRId32 ", name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }
}

TEST(TestSqliteTransaction, GetInserId) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto results = instance->executeQuery("select * from tb_getInsertId;");
    ASSERT_NE(nullptr, results);
    while (results->next()) {
        printf("id = %" PRId32 ", name = %s\n", (int) results->getInteger(0), results->getString(1).data());
    }

    auto count = instance->executeUpdate("insert into tb_getInsertId (name) values ('mike');");
    ASSERT_NE(0, count);

    int64_t id = 0;
    ASSERT_EQ(true, instance->getInsertId(id));
    printf("insertId = %" PRId64 "\n", id);

    auto results1 = instance->executeQuery("select * from tb_getInsertId;");
    ASSERT_NE(nullptr, results1);
    while (results1->next()) {
        printf("id = %" PRId32 ", name = %s\n", (int) results1->getInteger(0), results1->getString(1).data());
    }
}

TEST(TestSqliteTransaction, UseLess) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto a = instance->setAutoCommit(true);
}