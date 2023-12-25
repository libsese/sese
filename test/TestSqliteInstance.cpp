#include <sese/db/DriverManager.h>
#include <gtest/gtest.h>

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

TEST(TestDriverInstance, TestError) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto resultError = instance->executeQuery("select * from tb_query_error where id = 1;");
    ASSERT_EQ(nullptr, resultError);

    printf("errorMsg: %s\n", instance->getLastErrorMessage());

    //stmtError
    auto resultStmtError = instance->createStatement("select * from tb_stmt_query_error where id = 1;");
    ASSERT_EQ(nullptr, resultStmtError);

    printf("errorMsg: %s\n", instance->getLastErrorMessage());
}

// query
TEST(TestDriverInstance, TestQueryData) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_query where id = 1;");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("id = %d, name = %s Columns = %zu\n", (int) result->getInteger(0), result->getString(1).data(), result->getColumns());
    }

    result->reset();

    while (result->next()) {
        printf("id = %d, name = %s Columns = %zu\n", (int) result->getInteger(0), result->getString(1).data(), result->getColumns());
    }
}

// update
TEST(TestDriverInstance, TestUpdateData) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_update where id = 1;");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("result id = %d, name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    auto count = instance->executeUpdate(R"(update tb_update set name = "mike" where id = 1;)");
    ASSERT_NE(-1, count);

    auto result1 = instance->executeQuery("select * from tb_update where id = 1;");
    ASSERT_NE(nullptr, result1);
    while (result1->next()) {
        printf("result1 id = %d, name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

// delete
TEST(TestDriverInstance, TestDeleteData) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_delete;");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("result id = %d, name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    auto count = instance->executeUpdate("delete from tb_delete where id = 1;");
    EXPECT_NE(-1, count);

    auto result1 = instance->executeQuery("select * from tb_delete;");
    ASSERT_NE(nullptr, result1);
    while (result1->next()) {
        printf("result1 id = %d, name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

// insert
TEST(TestDriverInstance, TestInsertData) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_insert;");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("result id = %d, name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    auto count = instance->executeUpdate("insert into tb_insert (id, name) values (3, 'mike');");
    EXPECT_NE(-1, count);

    auto result1 = instance->executeQuery("select * from tb_insert;");
    ASSERT_NE(nullptr, result1);
    while (result1->next()) {
        printf("result1 id = %d, name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

// create update_stmt
TEST(TestDriverInstance, TestUpdateStmt) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_stmt_update;");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("result id = %d, name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    int64_t id = 1;
    const char *name = "mike";
    auto stmt = instance->createStatement("update tb_stmt_update set name = ? where id = ?;");
    ASSERT_NE(nullptr, stmt);

    EXPECT_EQ(true, stmt->setText(1, name));
    EXPECT_EQ(true, stmt->setLong(2, id));

    auto count = stmt->executeUpdate();
    EXPECT_NE(-1, count);

    id = 2;
    EXPECT_EQ(true, stmt->setNull(1));
    EXPECT_EQ(true, stmt->setLong(2, id));

    auto count1 = stmt->executeUpdate();
    ASSERT_NE(-1, count1);

    auto result1 = instance->executeQuery("select * from tb_stmt_update where id = 1;");
    while (result1->next()) {
        printf("result1 id = %d, name = %s\n", result1->getInteger(0), result1->getString(1).data());
    }
}

// create select_stmt
TEST(TestDriverInstance, TestSelectstmt) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    int64_t id = 1;
    auto stmt = instance->createStatement("select * from tb_stmt_query where id  = ?;");
    ASSERT_NE(nullptr, stmt);

    EXPECT_EQ(true, stmt->setLong(1, id));

    auto result = stmt->executeQuery();
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("stmt result: id = %d, name = %s, columns = %zu\n", (int) result->getInteger(0), result->getString(1).data(), result->getColumns());
    }

    result->reset();

    while (result->next()) {
        printf("stmt result: id = %d, name = %s, columns = %zu\n", (int) result->getInteger(0), result->getString(1).data(), result->getColumns());
    }
}

// create delete_stmt
TEST(TestDriverInstance, TestDeletestmt) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_stmt_delete;");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("stmt result: id = %d, name = %s\n", (int) result->getInteger(0), result->getString(1).data());
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
        printf("stmt result1: id = %d, name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

// create insert_stmt
TEST(TestDriverInstance, TestInsertstmt) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_stmt_insert;");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("stmt result: id = %d name = %s setDouble = %lf setFloat = %f setInteger = %d\n", (int) result->getLong(0), result->getString(1).data(), result->getDouble(2), result->getFloat(3), result->getInteger(4));
    }

    int32_t id = 3;
    const char *name = "mike";
    float flo = 5.1;
    double dou = 45.45;
    int64_t lon = 3;
    auto stmt = instance->createStatement("insert into tb_stmt_insert (id, name, setDouble, setFloat, setLong, setNull) values (?, ?, ?, ?, ?, ?);");
    ASSERT_NE(nullptr, stmt);

    ASSERT_EQ(true, stmt->setInteger(1, id));
    ASSERT_NE(true, stmt->setInteger(7, id));
    ASSERT_EQ(true, stmt->setText(2, name));
    ASSERT_NE(true, stmt->setText(7, name));
    ASSERT_EQ(true, stmt->setDouble(3, dou));
    ASSERT_NE(true, stmt->setDouble(7, dou));
    ASSERT_EQ(true, stmt->setFloat(4, flo));
    ASSERT_NE(true, stmt->setFloat(7, flo));
    ASSERT_EQ(true, stmt->setLong(5, lon));
    ASSERT_NE(true, stmt->setLong(7, lon));
    ASSERT_EQ(true, stmt->setNull(6));
    ASSERT_NE(true, stmt->setNull(7));

    auto count = stmt->executeUpdate();
    EXPECT_NE(-1, count);

    ASSERT_EQ(true, stmt->setInteger(1, id));
    ASSERT_EQ(true, stmt->setText(2, name));
    ASSERT_EQ(true, stmt->setDouble(3, dou));
    ASSERT_EQ(true, stmt->setFloat(4, flo));
    ASSERT_EQ(true, stmt->setLong(5, lon));
    ASSERT_EQ(true, stmt->setNull(6));

    auto count1 = stmt->executeUpdate();
    EXPECT_NE(-1, count1);

    ASSERT_EQ(true, stmt->setText(2, name));
    ASSERT_EQ(true, stmt->setInteger(1, id));
    ASSERT_EQ(true, stmt->setDouble(3, dou));
    ASSERT_EQ(true, stmt->setFloat(4, flo));
    ASSERT_EQ(true, stmt->setLong(5, lon));
    ASSERT_EQ(true, stmt->setNull(6));

    auto count2 = stmt->executeUpdate();
    EXPECT_NE(-1, count2);

    ASSERT_EQ(true, stmt->setDouble(3, dou));
    ASSERT_EQ(true, stmt->setText(2, name));
    ASSERT_EQ(true, stmt->setInteger(1, id));
    ASSERT_EQ(true, stmt->setFloat(4, flo));
    ASSERT_EQ(true, stmt->setLong(5, lon));
    ASSERT_EQ(true, stmt->setNull(6));

    auto count3 = stmt->executeUpdate();
    EXPECT_NE(-1, count3);

    ASSERT_EQ(true, stmt->setFloat(4, flo));
    ASSERT_EQ(true, stmt->setDouble(3, dou));
    ASSERT_EQ(true, stmt->setText(2, name));
    ASSERT_EQ(true, stmt->setInteger(1, id));
    ASSERT_EQ(true, stmt->setLong(5, lon));
    ASSERT_EQ(true, stmt->setNull(6));

    auto count4 = stmt->executeUpdate();
    EXPECT_NE(-1, count4);

    ASSERT_EQ(true, stmt->setLong(5, lon));
    ASSERT_EQ(true, stmt->setFloat(4, flo));
    ASSERT_EQ(true, stmt->setDouble(3, dou));
    ASSERT_EQ(true, stmt->setText(2, name));
    ASSERT_EQ(true, stmt->setInteger(1, id));
    ASSERT_EQ(true, stmt->setNull(6));


    auto stmtQuery = instance->createStatement("select id, name, setDouble, setFloat, setLong from tb_stmt_insert;");
    ASSERT_NE(nullptr, stmtQuery);

    auto result1 = stmtQuery->executeQuery();
    ASSERT_NE(nullptr, result1);
    while (result1->next()) {
        printf("result1: id = %d name = %s setDouble = %lf setFloat = %f setInteger = %d\n", (int) result1->getLong(0), result1->getString(1).data(), result1->getDouble(2), result1->getFloat(3), result1->getInteger(4));
    }
}

// get autoCommit
TEST(TestTransaction, TestGetAutoCommit) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    bool status;
    ASSERT_EQ(true, instance->getAutoCommit(status));
    printf("autoCommit = %d\n", status);
}

// begin
TEST(TestTransaction, TestBegin) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
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

    auto count1 = instance->executeUpdate("insert into tb_begin (id, name) values (4, 'mike');");
    ASSERT_NE(0, count1);

    ASSERT_EQ(true, instance->commit());

    auto results3 = instance->executeQuery("select * from tb_begin;");
    while (results3->next()) {
        printf("id = %d, name = %s\n", results3->getInteger(0), results3->getString(1).data());
    }
}

// commit
TEST(TestTransaction, TestCommit) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto results = instance->executeQuery("select * from tb_commit;");
    while (results->next()) {
        printf("id = %d, name = %s\n", results->getInteger(0), results->getString(1).data());
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
        printf("id = %d, name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }
}

// rollBack
TEST(TestTransaction, TestRollBack) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto results = instance->executeQuery("select * from tb_rollBack;");
    while (results->next()) {
        printf("id = %d, name = %s\n", results->getInteger(0), results->getString(1).data());
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
        printf("id = %d, name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }
}

// getInsertId
TEST(TestTransaction, TestGetInserId) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
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
    printf("insertId = %ld\n", id);

    auto results1 = instance->executeQuery("select * from tb_getInsertId;");
    ASSERT_NE(nullptr, results1);
    while (results1->next()) {
        printf("id = %d, name = %s\n", (int) results1->getInteger(0), results1->getString(1).data());
    }
}

TEST(TestTransaction, UseLess) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto a = instance->setAutoCommit(true);
}
