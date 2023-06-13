#include <sese/db/DriverManager.h>
#include <gtest/gtest.h>

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

// query
TEST(TestDriverInstance, TestQueryData) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance) << "Failed to create the database object instance";
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_query where id = 1;");
    ASSERT_NE(nullptr, result) << "Query failure";
    while (result->next()) {
        printf("id = %d, name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }
}

// update
TEST(TestDriverInstance, TestUpdateData) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance) << "Failed to create the database object instance";
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_update where id = 1;");
    ASSERT_NE(nullptr, result) << "Query failure";
    while (result->next()) {
        printf("result id = %d, name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    auto count = instance->executeUpdate(R"(update tb_update set name = "mike" where id = 1;)");
    EXPECT_NE(-1, count) << "Update failure";

    auto result1 = instance->executeQuery("select * from tb_update where id = 1;");
    ASSERT_NE(nullptr, result1) << "Query failure";
    while (result1->next()) {
        printf("result1 id = %d, name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

// delete
TEST(TestDriverInstance, TestDeleteData) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance) << "Failed to create the database object instance";
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_delete;");
    ASSERT_NE(nullptr, result) << "Query failure";
    while (result->next()) {
        printf("result id = %d, name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    auto count = instance->executeUpdate("delete from tb_delete where id = 1;");
    EXPECT_NE(-1, count) << "Delete failure";

    auto result1 = instance->executeQuery("select * from tb_delete;");
    ASSERT_NE(nullptr, result1) << "Query failure";
    while (result1->next()) {
        printf("result1 id = %d, name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

// insert
TEST(TestDriverInstance, TestInsertData) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance) << "Failed to create the database object instance";
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_delete;");
    ASSERT_NE(nullptr, result) << "Query failure";
    while (result->next()) {
        printf("result id = %d, name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    auto count = instance->executeUpdate("insert into tb_insert (id, name) values (3, 'mike');");
    EXPECT_NE(-1, count) << "Insertion failure";

    auto result1 = instance->executeQuery("select * from tb_delete;");
    ASSERT_NE(nullptr, result1) << "Query failure";
    while (result1->next()) {
        printf("result1 id = %d, name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

// create update_stmt
TEST(TestDriverInstance, TestUpdatestmt) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance) << "Failed to create the database object instance";
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_stmt_update where id = 1;");
    ASSERT_NE(nullptr, result) << "Query failure";
    while (result->next()) {
        printf("result id = %d, name = %s\n", (int) result->getInteger(0), result->getString(0).data());
    }

    int64_t id = 1;
    const char *name = "mike";
    auto stmt = instance->createStatement("update tb_stmt_update set name = ? where id = ?;");
    ASSERT_NE(nullptr, stmt) << "Failed to create update a preprocessed statement";

    EXPECT_EQ(true, stmt->setText(1, name)) << "Failed to fill in the text value parameter";
    EXPECT_EQ(true, stmt->setLong(2, id)) << "Failed to fill in the Long value parameter";

    auto count = stmt->executeUpdate();
    EXPECT_NE(-1, count) << "Failed to use the updated prepared statement";

    auto result1 = instance->executeQuery("select * from tb_stmt_update where id = 1;");
    ASSERT_NE(nullptr, result1) << "Query failure";
    while (result1->next()) {
        printf("result1 id = %d, name = %s\n", (int) result1->getInteger(0), result1->getString(0).data());
    }
}

// create select_stmt
TEST(TestDriverInstance, TestSelectstmt) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance) << "Failed to create the database object instance";
    ASSERT_EQ(0, instance->getLastError());

    int64_t id = 1;
    auto stmt = instance->createStatement("select * from tb_stmt_query where id = ?;");
    ASSERT_NE(nullptr, stmt) << "Failed to create a query preprocessing statement";

    EXPECT_EQ(true, stmt->setLong(1, id)) << "Failed to fill in the Long value parameter";

    auto result = stmt->executeQuery();
    ASSERT_NE(nullptr, result) << "Failed to use the query prepared statement";
    while (result->next()) {
        printf("stmt result: id = %d, name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }
}

// create delete_stmt
TEST(TestDriverInstance, TestDeletestmt) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance) << "Failed to create the database object instance";
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_stmt_delete;");
    ASSERT_NE(nullptr, result) << "Query failure";
    while (result->next()) {
        printf("stmt result: id = %d, name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    int64_t id = 1;
    auto stmt = instance->createStatement("delete from tb_stmt_delete where id = ?;");
    ASSERT_NE(nullptr, stmt) << "Failed to create delete a preprocessed statement";

    EXPECT_EQ(true, stmt->setLong(1, id)) << "Failed to fill in the Long value parameter";

    auto count = stmt->executeUpdate();
    EXPECT_NE(-1, count) << "Failed to use the delete prepared statement";

    auto result1 = instance->executeQuery("select * from tb_stmt_delete;");
    ASSERT_NE(nullptr, result1) << "Query failure";
    while (result1->next()) {
        printf("stmt result1: id = %d, name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

// create insert_stmt
TEST(TestDriverInstance, TestInsertstmt) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance) << "Failed to create the database object instance";
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_stmt_insert;");
    ASSERT_NE(nullptr, result) << "Query failure";
    while (result->next()) {
        printf("stmt result: id = %d, name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    int64_t id = 3;
    const char *name = "mike";
    auto stmt = instance->createStatement("insert into tb_stmt_insert (id, name) values (?, ?);");
    ASSERT_NE(nullptr, stmt) << "Failed to create insert a preprocessed statement";

    EXPECT_EQ(true, stmt->setLong(1, id)) << "Failed to fill in the Long value parameter";
    EXPECT_EQ(true, stmt->setText(2, name)) << "Failed to fill in the Test value parameter";

    auto count = stmt->executeUpdate();
    EXPECT_NE(-1, count) << "Failed to use the insert prepared statement";

    auto result1 = instance->executeQuery("select * from tb_stmt_insert;");
    ASSERT_NE(nullptr, result1) << "Query failure";
    while (result1->next()) {
        printf("stmt result1: id = %d, name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}