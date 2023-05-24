#include <sese/db/DriverManager.h>
#include <gtest/gtest.h>

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

auto instance = DriverManager::getInstance(
        DatabaseType::Maria,
        "host=127.0.0.1;user=root;pwd=qiuchenli;db=db_test;port=3306;"
);

TEST(TestDriverInstance, TestInstance) {
    ASSERT_NE(nullptr, instance) << "Failed to create the database object instance";
}

// 查某行数据
TEST(TestDriverInstance, TestQueryData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=qiuchenli;db=db_test;port=3306;"
    );

    auto result = instance->executeQuery("select name from tb_delete where id = 1;");
    ASSERT_NE(nullptr, result) << "Query failure";
    printf("result: id = %d name = %s\n", (int) result->getInteger(0), result->getString(1).data());
}

// 改
TEST(TestDriverInstance, TestModifyData) {
    auto result = instance->executeQuery("update id from tb_update where id = 1;");
    printf("result: id = %d name = %s\n", (int) result->getInteger(0), result->getString(1).data());

    auto count = instance->executeUpdate("update tb_update set name = 'bar' where id = 1;");
    EXPECT_NE(-1, count) << "Update failure";

    auto result1 = instance->executeQuery("select id from tb_update where id = 1;");
    printf("result1: id = %d name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
}

// 增
TEST(TestDriverInstance, TestInsertData) {
    auto result = instance->executeQuery("select * from tb_insert;");
    while (result->next()) {
        printf("result: id = %d name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    auto count2 = instance->executeUpdate("insert into tb_insert (id, name) values(3, 'mike');");
    EXPECT_NE(-1, count2) << "Insertion failure";

    auto result1 = instance->executeQuery("select * from tb_insert;");
    while (result1->next()) {
        printf("result: id = %d name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

// 删
TEST(TestDriverInstance, TestDeleteData) {
    auto result = instance->executeQuery("select * from tb_delete;");
    while (result->next()) {
        printf("result: id = %d name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    auto count3 = instance->executeUpdate("delete from tb_delete where id = 1;");
    EXPECT_NE(-1, count3) << "Deletion failure";

    auto result1 = instance->executeQuery("select * from tb_delete;");
    while (result1->next()) {
        printf("result: id = %d name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

// 创建查询的预处理语句
TEST(TestCreateStmt, TestQueryStmt) {
    int64_t value = 1;
    auto stmt = instance->createStatement("select name from tb_stmt_query where id = ?;");
    ASSERT_NE(nullptr, stmt) << "Failed to create a query preprocessing statement";

    EXPECT_EQ(true, stmt->setLong(1, value));

    auto result = stmt->executeQuery();
    EXPECT_NE(nullptr, result) << "Failed to use the query prepared statement";
}

// 创建更新的预处理语句
TEST(TestCreateStmt, TestUateStmt) {
    auto result = instance->executeQuery("select * from tb_stmt_update where id = 1;");
    printf("result: id = %d name = %s\n", (int) result->getInteger(0), result->getString(1).data());

    int64_t id = 1;
    const char *name = "mike";
    auto stmt1 = instance->createStatement("update tb_stmt_update set name = ? where id = ?;");
    ASSERT_NE(nullptr, stmt1) << "Failed to create update a preprocessed statement";

    EXPECT_EQ(true, stmt1->setText(1, name)) << "Failed to fill in the text value parameter";
    EXPECT_EQ(true, stmt1->setLong(2, id)) << "Failed to fill in the Long value parameter";

    auto count = stmt1->executeUpdate();
    EXPECT_NE(-1, count) << "Failed to use the updated prepared statement";

    auto result1 = instance->executeQuery("select * from tb_stmt_update where id = 1;");
    printf("result1: id = %d name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
}

// 创建删除的预处理语句
TEST(TestCreateStmt, TestdeleteStmt) {
    auto result = instance->executeQuery("select * from tb_stmt_delete;");
    while (result->next()){
        printf("result: id = %d name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    int64_t id = 1;
    auto stmt = instance->createStatement("delete tb_stmt_delete where id = ?;");
    ASSERT_NE(nullptr, stmt) << "Failed to create delete a preprocessed statement";

    EXPECT_EQ(true, stmt->setLong(2, id)) << "Failed to fill in the Long value parameter";

    auto count = stmt->executeUpdate();
    EXPECT_NE(-1, count) << "Failed to use the delete prepared statement";

    auto result1 = instance->executeQuery("select * from tb_stmt_delete;");
    while (result1->next()){
        printf("result1: id = %d name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

// 创建插入的预处理语句
TEST(TestCreateStmt, TestinsertStmt) {
    auto result = instance->executeQuery("select * from tb_stmt_insert;");
    while (result->next()){
        printf("result: id = %d name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    int64_t id = 1;
    auto stmt = instance->createStatement("delete tb_stmt_insert where id = ?;");
    ASSERT_NE(nullptr, stmt) << "Failed to create insert a preprocessed statement";

    EXPECT_EQ(true, stmt->setLong(2, id)) << "Failed to fill in the Long value parameter";

    auto count = stmt->executeUpdate();
    EXPECT_NE(-1, count) << "Failed to use the insert prepared statement";

    auto result1 = instance->executeQuery("select * from tb_stmt_insert;");
    while (result1->next()){
        printf("result1: id = %d name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}