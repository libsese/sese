#include <sese/db/DriverManager.h>
#include <gtest/gtest.h>
#include <cinttypes>

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

TEST(TestMariaStmt, QueryStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    int64_t id = 1;
    auto stmt = instance->createStatement("select * from tb_stmt_query where id = ?;");
    ASSERT_NE(nullptr, stmt);

    //    ASSERT_NE(true, stmt->setLong(2, id));
    ASSERT_EQ(true, stmt->setLong(1, id));

    auto result = stmt->executeQuery();
    ASSERT_NE(nullptr, result);
    printf("columns: %zu\n", result->getColumns());
    while (result->next()) {
        printf("result: id = %" PRId32 " name = %s\n", result->getInteger(0), result->getString(1).data());
    }

    result->reset();

    while (result->next()) {
        printf("result: id = %" PRId32 " name = %s\n", result->getInteger(0), result->getString(1).data());
    }
}

TEST(TestMariaStmt, UpdateStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_stmt_update where id = 1;");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("result: id = %" PRId32 " name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    int64_t id = 1;
    const char *name = "mike";
    auto stmt = instance->createStatement("update tb_stmt_update set name = ? where id = ?;");
    ASSERT_NE(nullptr, stmt);

    ASSERT_NE(true, stmt->setText(3, name));
    ASSERT_EQ(true, stmt->setText(1, name));
    ASSERT_EQ(true, stmt->setLong(2, id));

    auto count = stmt->executeUpdate();
    ASSERT_NE(-1, count);

    int64_t id1 = 2;
    ASSERT_NE(true, stmt->setNull(3));
    ASSERT_EQ(true, stmt->setNull(1));
    ASSERT_EQ(true, stmt->setLong(2, id1));
    auto count1 = stmt->executeUpdate();
    ASSERT_NE(-1, count1);

    auto result1 = instance->executeQuery("select * from tb_stmt_update where id = 1;");
    ASSERT_NE(nullptr, result1);
    while (result1->next()) {
        printf("result1: id = %" PRId32 " name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

TEST(TestMariaStmt, DeleteStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_stmt_delete;");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("result: id = %" PRId32 " name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    int32_t id = 1;
    auto stmt = instance->createStatement("delete from tb_stmt_delete where id = ?;");
    ASSERT_NE(nullptr, stmt);

    ASSERT_EQ(true, stmt->setInteger(1, id));

    auto count = stmt->executeUpdate();
    ASSERT_NE(-1, count);

    auto result1 = instance->executeQuery("select * from tb_stmt_delete;");
    ASSERT_NE(nullptr, result1);
    while (result1->next()) {
        printf("result1: id = %" PRId32 " name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

TEST(TestMariaStmt, InsertStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"

    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto stmtQuery = instance->createStatement("select * from tb_stmt_insert;");
    auto result = stmtQuery->executeQuery();
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("result: id = %" PRId32 " name = %s setDouble = %lf setFloat = %f setLong = %" PRId64 "\n", (int) result->getInteger(0), result->getString(1).data(), result->getDouble(2), result->getFloat(3), result->getLong(4));
    }

    int32_t id = 3;
    const char *name = "mike";
    int64_t Long = 78;
    float flo = 5.1f;
    double dou = 45.45;
    auto stmt = instance->createStatement("insert into tb_stmt_insert (id, name, setDouble, setFloat, setLong) values (?, ?, ?, ?, ?);");
    ASSERT_NE(nullptr, stmt);
    ASSERT_EQ(true, stmt->setInteger(1, id));
    ASSERT_NE(true, stmt->setInteger(6, id));
    ASSERT_EQ(true, stmt->setText(2, name));
    ASSERT_NE(true, stmt->setText(6, name));
    ASSERT_NE(true, stmt->setDouble(6, dou));
    ASSERT_EQ(true, stmt->setDouble(3, dou));
    ASSERT_NE(true, stmt->setFloat(6, flo));
    ASSERT_EQ(true, stmt->setFloat(4, flo));
    ASSERT_NE(true, stmt->setLong(6, Long));
    ASSERT_EQ(true, stmt->setLong(5, Long));

    auto count = stmt->executeUpdate();
    ASSERT_NE(-1, count);

    auto stmtQuery1 = instance->createStatement("select * from tb_stmt_insert;");
    auto result1 = stmtQuery1->executeQuery();
    ASSERT_NE(nullptr, result1);
    while (result1->next()) {
        printf("result1: id = %" PRId32 " name = %s setDouble = %lf setFloat = %f setLong = %" PRId64 "\n", (int) result1->getInteger(0), result1->getString(1).data(), result1->getDouble(2), result1->getFloat(3), result1->getLong(4));
    }
}

TEST(TestMariaStmt, GetDateTimeStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto stmt = instance->createStatement("select * from tb_stmt_getTime;");
    ASSERT_NE(nullptr, stmt);

    auto result = stmt->executeQuery();
    ASSERT_NE(nullptr, result);
    printf("columns: %zu\n", result->getColumns());
    while (result->next()) {
        printf("id = %" PRId32 " time = %" PRId64 "\n", result->getInteger(0), result->getDateTime(1).value().getTimestamp());
    }
}

TEST(TestMariaStmt, SetDateTimeStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_stmt_setTime;");
    ASSERT_NE(nullptr, result);

    printf("columns: %zu\n", result->getColumns());
    while (result->next()) {
        printf("id = %" PRId32 " time = %" PRId64 "\n", result->getInteger(0), result->getDateTime(1).value().getTimestamp());
    }

    auto stmt = instance->createStatement("insert into tb_stmt_setTime (id, time) values (?, ?);");
    ASSERT_NE(nullptr, stmt);

    int id = 3;
    auto dateTime = sese::DateTime(1679142600000000, 0);
    ASSERT_EQ(true, stmt->setInteger(1, id));
    ASSERT_NE(true, stmt->setInteger(6, id));
    ASSERT_EQ(true, stmt->setDateTime(2, dateTime));
    ASSERT_NE(true, stmt->setDateTime(6, dateTime));

    auto count = stmt->executeUpdate();
    ASSERT_NE(-1, count);

    stmt->setDateTime(2, dateTime);
    stmt->setInteger(2, id);
    stmt->setDateTime(2, dateTime);

    auto result1 = instance->executeQuery("select * from tb_stmt_setTime;");
    ASSERT_NE(nullptr, result1);

    printf("columns: %zu\n", result1->getColumns());
    while (result1->next()) {
        printf("id = %" PRId32 " time = %" PRId64 "\n", result1->getInteger(0), result1->getDateTime(1).value().getTimestamp());
    }
}
