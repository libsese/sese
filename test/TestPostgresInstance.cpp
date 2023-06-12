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
            "host=127.0.0.1;user=postgres;password=libsese;dbname=db_test;port=18080;"
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
            "host=127.0.0.1;user=postgres;password=libsese;dbname=db_test;port=18080;"
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
            "host=127.0.0.1;user=postgres;password=libsese;dbname=db_test;port=18080;"
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
            "host=127.0.0.1;user=postgres;password=libsese;dbname=db_test;port=18080;"
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
            "host=127.0.0.1;user=postgres;password=libsese;dbname=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);

    int32_t id = 1;
    auto stmt = instance->createStatement("select * from tb_stmt_query where id = ?;");
    ASSERT_NE(nullptr, stmt);

    ASSERT_EQ(true, stmt->setInteger(1, id));

    auto results = stmt->executeQuery();
    ASSERT_NE(nullptr, results);
    while (results->next()) {
        printf("id = %d, name = %s\n", (int) results->getInteger(0), results->getString(1).data());
    }

}

// create update_stmt
TEST(TestDriverInstance, TestUpdateStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;password=libsese;dbname=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    auto results = instance->executeQuery("select * from tb_stmt_update;");
    ASSERT_NE(nullptr, results);
    while (results->next()) {
        printf("id = %d, name = %s\n", (int) results->getInteger(0), results->getString(1).data());
    }

    int32_t id = 1;
    const char *name = "mike";
    auto stmt = instance->createStatement("update tb_stmt_update set name = ? where id = ?;");
    ASSERT_NE(nullptr, stmt);

    ASSERT_EQ(true, stmt->setInteger(2, id));
    ASSERT_EQ(true, stmt->setText(1, name));

    auto count = stmt->executeUpdate();
    ASSERT_NE(-1, count);

    auto results1 = instance->executeQuery("select * from tb_stmt_update;");
    ASSERT_NE(nullptr, results1);
    while (results1->next()) {
        printf("id = %d, name = %s\n", (int) results1->getInteger(0), results1->getString(1).data());
    }
}

// create delete_stmt
TEST(TestDriverInstance, TestDeleteStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;password=libsese;dbname=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    auto results = instance->executeQuery("select * from tb_stmt_delete;");
    ASSERT_NE(nullptr, results);
    while (results->next()) {
        printf("id = %d, name = %s\n", (int) results->getInteger(0), results->getString(1).data());
    }

    int32_t id = 1;
    auto stmt = instance->createStatement("delete from tb_stmt_delete where id = ?;");
    ASSERT_NE(nullptr, stmt);

    ASSERT_EQ(true, stmt->setInteger(1, id));

    auto count = stmt->executeUpdate();
    ASSERT_NE(-1, count);

    auto results1 = instance->executeQuery("select * from tb_stmt_delete;");
    ASSERT_NE(nullptr, results1);
    while (results1->next()) {
        printf("id = %d, name = %s\n", (int) results1->getInteger(0), results1->getString(1).data());
    }
}

// create insert_stmt
TEST(TestDriverInstance, TestInsertStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;password=libsese;dbname=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    auto results = instance->executeQuery("select * from tb_stmt_insert;");
    ASSERT_NE(nullptr, results);
    while (results->next()) {
        printf("id = %d, name = %s\n", (int) results->getInteger(0), results->getString(1).data());
    }

    int32_t id = 3;
    const char *name = "mike";
    auto stmt = instance->createStatement("insert into tb_stmt_insert (id, name) values (?, ?);");
    ASSERT_NE(nullptr, stmt);

    ASSERT_EQ(true, stmt->setInteger(1, id));
    ASSERT_EQ(true, stmt->setText(2, name));

    auto count = stmt->executeUpdate();
    ASSERT_NE(-1, count);

    auto results1 = instance->executeQuery("select * from tb_stmt_insert;");
    ASSERT_NE(nullptr, results1);
    while (results1->next()) {
        printf("id = %d, name = %s\n", (int) results1->getInteger(0), results1->getString(1).data());
    }
}
