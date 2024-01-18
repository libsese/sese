#include <sese/db/DriverManager.h>
#include <sese/util/Initializer.h>
#include <gtest/gtest.h>
#include <cinttypes>

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

TEST(TestPostgresTransaction, Commit) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    auto results = instance->executeQuery("select * from tb_commit;");
    while (results->next()) {
        printf("id = %" PRId32 ", name = %s\n", results->getInteger(0), results->getString(1).data());
    }

    ASSERT_EQ(true, instance->begin());

    auto count = instance->executeUpdate("insert into tb_commit (id, name) values (3, 'mike')");
    ASSERT_NE(-1, count);

    ASSERT_EQ(true, instance->commit());

    auto results1 = instance->executeQuery("select * from tb_commit;");
    while (results1->next()) {
        printf("id = %" PRId32 ", name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }
}

TEST(TestPostgresTransaction, RollBack) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    ASSERT_EQ(true, instance->begin());

    auto results = instance->executeQuery("select * from tb_rollBack;");
    while (results->next()) {
        printf("id = %" PRId32 ", name = %s\n", results->getInteger(0), results->getString(1).data());
    }

    auto count = instance->executeUpdate("insert into tb_rollBack (id, name) values (3, 'mike');");
    ASSERT_NE(-1, count);

    ASSERT_EQ(true, instance->rollback());

    auto results1 = instance->executeQuery("select * from tb_rollBack;");
    while (results1->next()) {
        printf("id = %" PRId32 ", name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }
}

TEST(TestPostgresTransaction, Begin) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    // 事务的回滚操作
    auto results = instance->executeQuery("select * from tb_begin;");
    while (results->next()) {
        printf("id = %" PRId32 ", name = %s\n", results->getInteger(0), results->getString(1).data());
    }

    ASSERT_EQ(true, instance->begin());

    auto count = instance->executeUpdate("insert into tb_begin (id, name) values (3, 'mike');");
    ASSERT_NE(-1, count);

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
    ASSERT_NE(-1, count1);

    ASSERT_EQ(true, instance->commit());

    auto results3 = instance->executeQuery("select * from tb_begin;");
    while (results3->next()) {
        printf("id = %" PRId32 ", name = %s\n", results3->getInteger(0), results3->getString(1).data());
    }
}

TEST(TestPostgresTransaction, UseLess) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
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