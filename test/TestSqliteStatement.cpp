#include <sese/db/DriverManager.h>
#include <sese/util/Initializer.h>
#include <gtest/gtest.h>
#include <cinttypes>

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

TEST(TestSqliteStmt, UpdateStmt) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_stmt_update;");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("result id = %" PRId32 ", name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    int64_t id = 1;
    const char *name = "mike";
    auto stmt = instance->createStatement("update tb_stmt_update set name = ? where id = ?;");
    ASSERT_NE(nullptr, stmt);

    ASSERT_EQ(true, stmt->setText(1, name));
    ASSERT_EQ(true, stmt->setLong(2, id));

    auto count = stmt->executeUpdate();
    ASSERT_NE(-1, count);

    id = 2;
    ASSERT_EQ(true, stmt->setNull(1));
    ASSERT_EQ(true, stmt->setLong(2, id));

    auto count1 = stmt->executeUpdate();
    ASSERT_NE(-1, count1);

    auto result1 = instance->executeQuery("select * from tb_stmt_update where id = 1;");
    while (result1->next()) {
        printf("result1 id = %" PRId32 ", name = %s\n", result1->getInteger(0), result1->getString(1).data());
    }
}

TEST(TestSqliteStmt, Selectstmt) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    int32_t id = 1;
    auto stmt = instance->createStatement("select * from tb_stmt_query where id  = ?;");
    ASSERT_NE(nullptr, stmt);

    ASSERT_EQ(true, stmt->setInteger(1, id));

    auto result = stmt->executeQuery();
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("stmt result: id = %" PRId32 ", name = %s, columns = %zu\n", (int) result->getInteger(0), result->getString(1).data(), result->getColumns());
    }

    result->reset();

    while (result->next()) {
        printf("stmt result: id = %" PRId32 ", name = %s, columns = %zu\n", (int) result->getInteger(0), result->getString(1).data(), result->getColumns());
    }
}

TEST(TestSqliteStmt, Deletestmt) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_stmt_delete;");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("stmt result: id = %" PRId32 ", name = %s\n", (int) result->getInteger(0), result->getString(1).data());
    }

    int64_t id = 1;
    auto stmt = instance->createStatement("delete from tb_stmt_delete where id = ?;");
    ASSERT_NE(nullptr, stmt);

    ASSERT_EQ(true, stmt->setLong(1, id));

    auto count = stmt->executeUpdate();
    ASSERT_NE(-1, count);

    auto result1 = instance->executeQuery("select * from tb_stmt_delete;");
    ASSERT_NE(nullptr, result1);
    while (result1->next()) {
        printf("stmt result1: id = %" PRId32 ", name = %s\n", (int) result1->getInteger(0), result1->getString(1).data());
    }
}

TEST(TestSqliteStmt, Insertstmt) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_stmt_insert;");
    ASSERT_NE(nullptr, result);
    while (result->next()) {
        printf("stmt result: id = %" PRId32 " name = %s setDouble = %lf setFloat = %f setLong = %" PRId64 "\n", (int) result->getInteger(0), result->getString(1).data(), result->getDouble(2), result->getFloat(3), result->getLong(4));
    }

    int32_t id = 3;
    const char *name = "mike";
    float flo = 5.1f;
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
    ASSERT_NE(-1, count);

    ASSERT_EQ(true, stmt->setInteger(1, id));
    ASSERT_EQ(true, stmt->setText(2, name));
    ASSERT_EQ(true, stmt->setDouble(3, dou));
    ASSERT_EQ(true, stmt->setFloat(4, flo));
    ASSERT_EQ(true, stmt->setLong(5, lon));
    ASSERT_EQ(true, stmt->setNull(6));

    auto count1 = stmt->executeUpdate();
    ASSERT_NE(-1, count1);

    ASSERT_EQ(true, stmt->setText(2, name));
    ASSERT_EQ(true, stmt->setInteger(1, id));
    ASSERT_EQ(true, stmt->setDouble(3, dou));
    ASSERT_EQ(true, stmt->setFloat(4, flo));
    ASSERT_EQ(true, stmt->setLong(5, lon));
    ASSERT_EQ(true, stmt->setNull(6));

    auto count2 = stmt->executeUpdate();
    ASSERT_NE(-1, count2);

    ASSERT_EQ(true, stmt->setDouble(3, dou));
    ASSERT_EQ(true, stmt->setText(2, name));
    ASSERT_EQ(true, stmt->setInteger(1, id));
    ASSERT_EQ(true, stmt->setFloat(4, flo));
    ASSERT_EQ(true, stmt->setLong(5, lon));
    ASSERT_EQ(true, stmt->setNull(6));

    auto count3 = stmt->executeUpdate();
    ASSERT_NE(-1, count3);

    ASSERT_EQ(true, stmt->setFloat(4, flo));
    ASSERT_EQ(true, stmt->setDouble(3, dou));
    ASSERT_EQ(true, stmt->setText(2, name));
    ASSERT_EQ(true, stmt->setInteger(1, id));
    ASSERT_EQ(true, stmt->setLong(5, lon));
    ASSERT_EQ(true, stmt->setNull(6));

    auto count4 = stmt->executeUpdate();
    ASSERT_NE(-1, count4);

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
        printf("result1: id = %" PRId32 " name = %s setDouble = %lf setFloat = %f setInteger = %" PRId64 "\n", (int) result1->getInteger(0), result1->getString(1).data(), result1->getDouble(2), result1->getFloat(3), result1->getLong(4));
    }
}