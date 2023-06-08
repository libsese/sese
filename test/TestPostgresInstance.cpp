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
            "host=127.0.0.1;user=postgres;password=qiuchenli;dbname=db_test;port=5432;"
    );
    ASSERT_NE(nullptr, instance);
    auto results = instance->executeQuery("select * from tb_query;");
    ASSERT_NE(nullptr, results);
    while (results->next()) {
        printf("id = %d, name = %s\n", (int) results->getInteger(0), results->getString(1).data());
    }
}

// update
TEST(TestDriverInstance, TestUpdateData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;password=qiuchenli;dbname=db_test;port=5432;"
    );
    ASSERT_NE(nullptr, instance);

    auto results = instance->executeQuery("select * from tb_update;");
    while (results->next()) {
        printf("id = %d, name = %s\n", (int) results->getInteger(0), results->getString(1).data());
    }

    auto count = instance->executeUpdate("update tb_update set name = 'mike' where id = 1;");
    ASSERT_NE(0, count);

    auto results1 = instance->executeQuery("select * from tb_update;");
    while (results1->next()) {
        printf("id = %d, name = %s\n", (int) results1->getInteger(0), results1->getString(1).data());
    }
}

// delete
TEST(TestDriverInstance, TestDeleteData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;password=qiuchenli;dbname=db_test;port=5432;"
    );
    ASSERT_NE(nullptr, instance);

    auto results = instance->executeQuery("select * from tb_delete;");
    while (results->next()) {
        printf("id = %d, name = %s\n", (int) results->getInteger(0), results->getString(1).data());
    }

    auto count = instance->executeUpdate("delete from tb_delete where id = 1;");
    ASSERT_NE(0, count);

    auto results1 = instance->executeQuery("select * from tb_delete;");
    while (results1->next()) {
        printf("id = %d, name = %s\n", (int) results1->getInteger(0), results1->getString(1).data());
    }
}

// insert
TEST(TestDriverInstance, TestInsertData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;password=qiuchenli;dbname=db_test;port=5432;"
    );
    ASSERT_NE(nullptr, instance);

    auto results = instance->executeQuery("select * from tb_insert;");
    while (results->next()) {
        printf("id = %d, name = %s\n", (int) results->getInteger(0), results->getString(1).data());
    }

    auto count = instance->executeUpdate("insert into tb_insert (id, name) values (3,'mike');");
    ASSERT_NE(0, count);

    auto results1 = instance->executeQuery("select * from tb_insert;");
    while (results1->next()) {
        printf("id = %d, name = %s\n", (int) results1->getInteger(0), results1->getString(1).data());
    }
}

// create select_stmt
TEST(TestDriverInstance, TestQueryStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;password=qiuchenli;dbname=db_test;port=5432;"
    );
    ASSERT_NE(nullptr, instance);

    int64_t id = 1;
    auto stmt = instance->createStatement("select * from tb_insert where id = ?;");
    ASSERT_NE(nullptr, stmt);

    ASSERT_EQ(true, stmt->setLong(1, id));

    auto results = stmt->executeQuery();
    ASSERT_NE(nullptr, results);
    while (results->next()) {
        printf("id = %d, name = %s\n", (int) results->getInteger(0), results->getString(1).data());
    }

}

