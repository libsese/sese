#include <sese/db/DriverManager.h>
#include <gtest/gtest.h>

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;


// query
TEST(TestDriverInstance, TestQueryData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_query where id = 1");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("result id = %d name = %s\n", result->getInteger(0), result->getString(1).data());
    }
}

// update
TEST(TestDriverInstance, TestModifyData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_update where id = 1;");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("result: id = %d name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    auto count = instance->executeUpdate("update tb_update set name = 'bar' where id = 1;");
    EXPECT_NE(-1, count);

    auto result1 = instance->executeQuery("select * from tb_update where id = 1;");
    ASSERT_NE(nullptr, result1);
    while (result1->next()) {
        printf("result1: id = %d name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

// insert
TEST(TestDriverInstance, TestInsertData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_insert;");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("result: id = %d name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    auto count = instance->executeUpdate("insert into tb_insert (id, name) values (3, 'mike');");
    EXPECT_NE(-1, count);

    auto result1 = instance->executeQuery("select * from tb_insert;");
    ASSERT_NE(nullptr, result1);
    while (result1->next()) {
        printf("result1: id = %d name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

// delete
TEST(TestDriverInstance, TestDeleteData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_delete;");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("result: id = %d name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    auto count = instance->executeUpdate("delete from tb_delete where id = 1;");
    EXPECT_NE(-1, count);

    auto result1 = instance->executeQuery("select * from tb_delete;");
    ASSERT_NE(nullptr, result1);
    while (result1->next()) {
        printf("result1: id = %d name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

// create select_stmt
TEST(TestCreateStmt, TestQueryStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    int64_t id = 1;
    auto stmt = instance->createStatement("select * from tb_stmt_query where id = ?;");
    ASSERT_NE(nullptr, stmt);

    EXPECT_EQ(true, stmt->setLong(1, id));

    auto result = stmt->executeQuery();
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("result: id = %d, name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }
}

// create update_stmt
TEST(TestCreateStmt, TestUpdateStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_stmt_update where id = 1;");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("result: id = %d name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    int64_t id = 1;
    const char *name = "mike";
    auto stmt = instance->createStatement("update tb_stmt_update set name = ? where id = ?;");
    ASSERT_NE(nullptr, stmt);

    EXPECT_EQ(true, stmt->setText(1, name));
    EXPECT_EQ(true, stmt->setLong(2, id));

    auto count = stmt->executeUpdate();
    EXPECT_NE(-1, count);

    auto result1 = instance->executeQuery("select * from tb_stmt_update where id = 1;");
    ASSERT_NE(nullptr, result1);
    while (result1->next()) {
        printf("result1: id = %d name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

// create delete_stmt
TEST(TestCreateStmt, TestdeleteStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_stmt_delete;");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("result: id = %d name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    int64_t id = 1;
    auto stmt = instance->createStatement("delete from tb_stmt_delete where id = ?;");
    ASSERT_NE(nullptr, stmt);

    EXPECT_EQ(true, stmt->setLong(1, id));

    auto count = stmt->executeUpdate();
    EXPECT_NE(-1, count);

    auto result1 = instance->executeQuery("select * from tb_stmt_delete;");
    ASSERT_NE(nullptr, result1);
    while (result1->next()) {
        printf("result1: id = %d name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

// create insert_stmt
TEST(TestCreateStmt, TestinsertStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"

    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_stmt_insert;");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("result: id = %d name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    int64_t id = 3;
    const char *name = "mike";
    auto stmt = instance->createStatement("insert into tb_stmt_insert (id, name) values (?, ?);");
    ASSERT_NE(nullptr, stmt);

    EXPECT_EQ(true, stmt->setLong(1, id));
    EXPECT_EQ(true, stmt->setText(2, name));

    auto count = stmt->executeUpdate();
    EXPECT_NE(-1, count);

    auto result1 = instance->executeQuery("select * from tb_stmt_insert;");
    ASSERT_NE(nullptr, result1);
    while (result1->next()) {
        printf("result1: id = %d name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

// get autoCommit
TEST(TestTransaction, TestGetAutoCommit) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"

    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    ASSERT_EQ(true, instance->getAutoCommit());
}

// set autoCommit
TEST(TestTransaction, TestSetAutoCommit) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"

    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    ASSERT_EQ(true, instance->getAutoCommit());
    ASSERT_EQ(true, instance->setAutoCommit(false));

    ASSERT_EQ(true, instance->getAutoCommit());

    ASSERT_EQ(true, instance->setAutoCommit(true));
    ASSERT_EQ(true, instance->getAutoCommit());
}

// commit
TEST(TestTransaction, TestCommit) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"

    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto results = instance->executeQuery("select * from tb_commit;");
    while (results->next()) {
        printf("id = %d, name = %s\n", results->getInteger(0), results->getString(1).data());
    }

    ASSERT_EQ(true, instance->setAutoCommit(false));
    ASSERT_EQ(true, instance->getAutoCommit());

    auto count = instance->executeUpdate("insert into tb_commit (id, name) values (3, 'mike')");
    ASSERT_NE(0, count);

    ASSERT_EQ(true, instance->commit());

    auto results1 = instance->executeQuery("select * from tb_commit;");
    while (results1->next()) {
        printf("id = %d, name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }
}

// rollBack
TEST(TestTransaction, TestRollBack) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"

    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    ASSERT_EQ(true, instance->setAutoCommit(false));
    ASSERT_EQ(true, instance->getAutoCommit());

    auto results = instance->executeQuery("select * from tb_rollBack;");
    while (results->next()) {
        printf("id = %d, name = %s\n", results->getInteger(0), results->getString(1).data());
    }

    auto count = instance->executeUpdate("insert into tb_rollBack (id, name) values (3, 'mike');");
    ASSERT_NE(0, count);

    ASSERT_EQ(true, instance->rollback());

    auto results1 = instance->executeQuery("select * from tb_rollBack;");
    while (results1->next()) {
        printf("id = %d, name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }
}

// begin
TEST(TestTransaction, TestBegin) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"

    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    // 事务的回滚操作
    auto results = instance->executeQuery("select * from tb_begin;");
    while (results->next()) {
        printf("id = %d, name = %s\n", results->getInteger(0), results->getString(1).data());
    }

    ASSERT_EQ(true, instance->begin());

    auto count = instance->executeUpdate("insert into tb_begin (id, name) values (3, 'mike');");
    ASSERT_NE(0, count);

    ASSERT_EQ(true, instance->rollback());

    auto results1 = instance->executeQuery("select * from tb_begin;");
    while (results1->next()) {
        printf("id = %d, name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }

    // 事务的提交操作
    auto results2 = instance->executeQuery("select * from tb_begin;");
    while (results2->next()) {
        printf("id = %d, name = %s\n", results2->getInteger(0), results2->getString(1).data());
    }

    ASSERT_EQ(true, instance->begin());

    auto count1 = instance->executeUpdate("insert into tb_begin (id, name) values (3, 'mike');");
    ASSERT_NE(0, count1);

    ASSERT_EQ(true, instance->commit());

    auto results3 = instance->executeQuery("select * from tb_begin;");
    while (results3->next()) {
        printf("id = %d, name = %s\n", results3->getInteger(0), results3->getString(1).data());
    }
}

// getInsertId
TEST(TestTransaction, TestGetInserId) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"

    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto results = instance->executeQuery("select * from tb_getInsertId;");
    ASSERT_NE(nullptr, results);
    while (results->next()) {
        printf("id = %d, name = %s\n", (int) results->getInteger(0), results->getString(1).data());
    }

    auto count = instance->executeUpdate("insert into tb_getInsertId (name) values ('mike');");
    ASSERT_NE(0, count);

    int64_t id = 0;
    ASSERT_EQ(true, instance->getInsertId(id));
    printf("insertId = %lld\n", id);

    auto results1 = instance->executeQuery("select * from tb_getInsertId;");
    ASSERT_NE(nullptr, results1);
    while (results1->next()) {
        printf("id = %d, name = %s\n", (int) results1->getInteger(0), results1->getString(1).data());
    }
}
