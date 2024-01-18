#include <sese/db/DriverManager.h>
#include <sese/util/Initializer.h>
#include <gtest/gtest.h>
#include <cinttypes>

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

TEST(TestPostgresStmt, QueryStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    int32_t id = 1;
    auto stmt = instance->createStatement("select * from tb_stmt_query where id = ?;");
    ASSERT_NE(nullptr, stmt);

    ASSERT_EQ(true, stmt->setInteger(1, id));

    auto results = stmt->executeQuery();
    ASSERT_NE(nullptr, results);
    while (results->next()) {
        printf("id = %" PRId32 ", name = %s\n", results->getInteger(0), results->getString(1).data());
    }
}

TEST(TestPostgresStmt, UpdateStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    auto results = instance->executeQuery("select * from tb_stmt_update;");
    ASSERT_NE(nullptr, results);
    while (results->next()) {
        printf("id = %" PRId32 ", name = %s\n", results->getInteger(0), results->getString(1).data());
    }

    int32_t id = 1;
    const char *name = "mike";
    auto stmt = instance->createStatement("update tb_stmt_update set name = ? where id = ?;");
    ASSERT_NE(nullptr, stmt);

    ASSERT_EQ(true, stmt->setInteger(2, id));
    ASSERT_EQ(true, stmt->setText(1, name));
    //    ASSERT_EQ(true, stmt->setNull(1));

    auto count = stmt->executeUpdate();
    ASSERT_NE(-1, count);

    int32_t id1 = 2;
    ASSERT_EQ(true, stmt->setInteger(2, id1));
    ASSERT_NE(true, stmt->setNull(3));
    ASSERT_EQ(true, stmt->setNull(1));
    auto count1 = stmt->executeUpdate();
    ASSERT_NE(-1, count1);
    auto results1 = instance->executeQuery("select * from tb_stmt_update;");
    while (results1->next()) {
        printf("id = %" PRId32 ", name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }

}

TEST(TestPostgresStmt, DeleteStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    auto results = instance->executeQuery("select * from tb_stmt_delete;");
    ASSERT_NE(nullptr, results);
    while (results->next()) {
        printf("id = %" PRId32 ", name = %s\n", results->getInteger(0), results->getString(1).data());
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
        printf("id = %" PRId32 ", name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }
}

TEST(TestPostgresStmt, InsertStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    auto stmtQuery = instance->createStatement("select * from tb_stmt_insert;");
    auto result = stmtQuery->executeQuery();
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("result: id = %" PRId64 " name = %s setDouble = %lf setFloat = %f setInteger = %" PRId32 "\n", result->getLong(0), result->getString(1).data(), result->getDouble(2), result->getFloat(3), result->getInteger(4));
    }

    int64_t id = 3;
    const char *name = "mike";
    int32_t integer = 78;
    float flo = 5.1f;
    double dou = 45.45;
    auto stmt = instance->createStatement("insert into tb_stmt_insert (id, name, setDouble, setFloat, setInteger) values (?, ?, ?, ?, ?)");
    ASSERT_NE(nullptr, stmt);
    ASSERT_EQ(true, stmt->setLong(1, id));
    ASSERT_NE(true, stmt->setLong(6, id));
    ASSERT_EQ(true, stmt->setText(2, name));
    ASSERT_NE(true, stmt->setText(6, name));
    ASSERT_EQ(true, stmt->setDouble(3, dou));
    ASSERT_NE(true, stmt->setDouble(6, dou));
    ASSERT_EQ(true, stmt->setFloat(4, flo));
    ASSERT_NE(true, stmt->setFloat(6, flo));
    ASSERT_EQ(true, stmt->setInteger(5, integer));
    ASSERT_NE(true, stmt->setInteger(6, integer));

    auto count = stmt->executeUpdate();
    ASSERT_NE(-1, count);

    auto stmtQuery1 = instance->createStatement("select * from tb_stmt_insert;");
    auto result1 = stmtQuery1->executeQuery();
    ASSERT_NE(nullptr, result1);
    while (result1->next()) {
        printf("result1: id = %" PRId64 " name = %s setDouble = %lf setFloat = %f setInteger = %" PRId32 "\n", result1->getLong(0), result1->getString(1).data(), result1->getDouble(2), result1->getFloat(3), result1->getInteger(4));
    }
}