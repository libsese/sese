#include <sese/db/DriverManager.h>
#include <sese/util/Initializer.h>
#include <gtest/gtest.h>
#include <cinttypes>

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

// instanceError
TEST(TestDriverInstance, TestInstanceError) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=error;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_NE(instance->getLastError(), 0);
    printf("errorMsg:%s\n", instance->getLastErrorMessage());

    auto results = instance->executeQuery("select * from tb_query;");

    auto results1 = instance->executeUpdate("update tb_update set name = 'mike' where id = 1;");

    auto stmt = instance->createStatement("select * from tb_query;");
    auto results2 = stmt->executeQuery();

    auto stmt2 = instance->createStatement("update tb_update set name = 'mike' where id = 1;");
    auto results3 = stmt->executeUpdate();

    auto results4 = instance->commit();

    auto results5 = instance->rollback();

    auto results6 = instance->begin();
}

// query
TEST(TestDriverInstance, TestQueryData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);
    printf("errorMsg:%s\n", instance->getLastErrorMessage());

    // error
    auto results = instance->executeQuery("select * from tb_query_error;");
    ASSERT_EQ(nullptr, results);
    ASSERT_NE(instance->getLastError(), 0);
    printf("errorMsg: %s\n", instance->getLastErrorMessage());

    auto results1 = instance->executeQuery("select * from tb_query;");
    ASSERT_NE(nullptr, results1);
    printf("Columns = %zu", results1->getColumns());
    while (results1->next()) {
        printf("id = %" PRId32 ", name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }

    results1->reset();
    while (results1->next()) {
        printf("id = %" PRId32 ", name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }

    results1 = instance->executeQuery("select * from tb_query where id = 1;");
    ASSERT_NE(nullptr, results1);
    while (results1->next()) {
        printf("id = %" PRId32 ", name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }
}

// update
TEST(TestDriverInstance, TestUpdateData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    auto results = instance->executeQuery("select * from tb_update;");
    while (results->next()) {
        printf("id = %" PRId32 ", name = %s\n", (int) results->getInteger(0), results->getString(1).data());
    }

    auto count = instance->executeUpdate("update tb_update set name = 'mike' where id = 1;");
    ASSERT_NE(-1, count);

    count = instance->executeUpdate("update tb_update set name = 'bar' where id = 2;");
    ASSERT_NE(-1, count);

    // error
    auto count1 = instance->executeUpdate("update tb_update_error set name = 'mike' where id = 1;");
    ASSERT_EQ(-1, count1);
    ASSERT_NE(instance->getLastError(), 0);
    printf("errorMsg: %s\n", instance->getLastErrorMessage());

    auto results1 = instance->executeQuery("select * from tb_update;");
    while (results1->next()) {
        printf("id = %" PRId32 ", name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }
}

// delete
TEST(TestDriverInstance, TestDeleteData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    auto results = instance->executeQuery("select * from tb_delete;");
    while (results->next()) {
        printf("id = %" PRId32 ", name = %s\n", results->getInteger(0), results->getString(1).data());
    }

    auto count = instance->executeUpdate("delete from tb_delete where id = 1;");
    ASSERT_NE(-1, count);

    auto results1 = instance->executeQuery("select * from tb_delete;");
    ASSERT_NE(nullptr, results1);
    while (results1->next()) {
        printf("id = %" PRId32 ", name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }
}

// insert
TEST(TestDriverInstance, TestInsertData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    auto results = instance->executeQuery("select * from tb_insert;");
    ASSERT_NE(nullptr, results);
    while (results->next()) {
        printf("id = %" PRId32 ", name = %s\n", results->getInteger(0), results->getString(1).data());
    }

    auto count = instance->executeUpdate("insert into tb_insert (id, name) values (3,'mike');");
    ASSERT_NE(-1, count);

    auto results1 = instance->executeQuery("select * from tb_insert;");
    ASSERT_NE(nullptr, results1);
    while (results1->next()) {
        printf("id = %" PRId32 ", name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }
}

// commit
TEST(TestTransaction, TestCommit) {
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

// rollBack
TEST(TestTransaction, TestRollBack) {
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

// begin
TEST(TestTransaction, TestBegin) {
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

TEST(TestTransaction, UseLess) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    auto stmt = instance->createStatement("select * from tb_query_error;");
    auto results1 = stmt->executeQuery();

    auto stmt2 = instance->createStatement("update tb_update set name = 'mike' where id = 1;");
    auto results2 = stmt->executeUpdate();

    auto a = instance->setAutoCommit(true);

    bool b1;
    auto b = instance->getAutoCommit(b1);

    int64_t c1;
    auto c = instance->getInsertId(c1);
}
