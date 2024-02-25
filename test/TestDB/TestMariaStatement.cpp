#include <sese/db/DriverManager.h>
#include <gtest/gtest.h>
#include <cinttypes>

#include "Config.h"

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

TEST(TestMariaStmt, QueryStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            MYSQL_CONNECTION_STRING
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
    ASSERT_EQ(2, result->getColumns());
    if (result->next()) {
        ASSERT_EQ(1, result->getInteger(0));
        std::string_view strings = "foo";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    ASSERT_EQ(false, result->next());

    result->reset();

    if (result->next()) {
        ASSERT_EQ(1, result->getInteger(0));
        std::string_view strings = "foo";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    ASSERT_EQ(false, result->next());
}

TEST(TestMariaStmt, UpdateStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            MYSQL_CONNECTION_STRING
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    int64_t id = 1;
    const char *name = "mike";
    auto stmt = instance->createStatement("update tb_stmt_update set name = ? where id = ?;");
    ASSERT_NE(nullptr, stmt);

    ASSERT_NE(true, stmt->setText(3, name));
    ASSERT_EQ(true, stmt->setText(1, name));
    ASSERT_EQ(true, stmt->setLong(2, id));

    auto count = stmt->executeUpdate();
    ASSERT_NE(-1, count);

    auto result = instance->executeQuery("select * from tb_stmt_update where id = 1;");
    ASSERT_NE(nullptr, result);

    if (result->next()) {
        ASSERT_EQ(1, result->getInteger(0));
        ASSERT_NE(nullptr, result->getString(1).data());
    }
    ASSERT_EQ(false, result->next());

    int64_t id1 = 2;
    ASSERT_NE(true, stmt->setNull(3));
    ASSERT_EQ(true, stmt->setNull(1));
    ASSERT_EQ(true, stmt->setLong(2, id1));
    auto count1 = stmt->executeUpdate();
    ASSERT_NE(-1, count1);

    auto result1 = instance->executeQuery("select * from tb_stmt_update where id = 2;");
    ASSERT_NE(nullptr, result1);
    if (result1->next()) {
        ASSERT_EQ(2, result1->getInteger(0));
    }
    ASSERT_EQ(false, result1->next());
}

TEST(TestMariaStmt, DeleteStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            MYSQL_CONNECTION_STRING
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    int32_t id = 1;
    auto stmt = instance->createStatement("delete from tb_stmt_delete where id = ?;");
    ASSERT_NE(nullptr, stmt);

    ASSERT_EQ(true, stmt->setInteger(1, id));

    auto count = stmt->executeUpdate();
    ASSERT_NE(-1, count);

    auto result = instance->executeQuery("select * from tb_stmt_delete;");
    ASSERT_NE(nullptr, result);
    if (result->next()) {
        ASSERT_EQ(2, result->getInteger(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    ASSERT_EQ(false, result->next());
}

TEST(TestMariaStmt, InsertStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            MYSQL_CONNECTION_STRING

    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

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

    auto stmt_query = instance->createStatement("select * from tb_stmt_insert;");
    auto result = stmt_query->executeQuery();
    ASSERT_NE(nullptr, result);
    if (result->next()) {
        ASSERT_EQ(1, result->getInteger(0));
        std::string_view strings = "foo";
        ASSERT_EQ(strings, result->getString(1).data());
        ASSERT_EQ(1.000000, result->getDouble(2));
        ASSERT_EQ(1.000000, result->getFloat(3));
        ASSERT_EQ(1, result->getLong(4));
    }
    if (result->next()) {
        ASSERT_EQ(2, result->getInteger(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, result->getString(1).data());
        ASSERT_EQ(2.000000, result->getDouble(2));
        ASSERT_EQ(2.000000, result->getFloat(3));
        ASSERT_EQ(2, result->getLong(4));
    }
    if (result->next()) {
        ASSERT_EQ(3, result->getInteger(0));
        std::string_view strings = "mike";
        ASSERT_EQ(strings, result->getString(1).data());
        ASSERT_EQ(45.450000, result->getDouble(2));
        ASSERT_EQ(5.1f, result->getFloat(3));
        ASSERT_EQ(78, result->getLong(4));
    }
    ASSERT_EQ(false, result->next());
}

TEST(TestMariaStmt, GetDateTimeStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            MYSQL_CONNECTION_STRING
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto stmt = instance->createStatement("select * from tb_stmt_getTime;");
    ASSERT_NE(nullptr, stmt);

    auto result = stmt->executeQuery();
    ASSERT_NE(nullptr, result);
    if (result->next()) {
        ASSERT_EQ(1, result->getInteger(0));
        ASSERT_EQ(1679142600000000, result->getDateTime(1).value().getTimestamp());
    }
    if (result->next()) {
        ASSERT_EQ(2, result->getInteger(0));
        ASSERT_EQ(1679142600000000, result->getDateTime(1).value().getTimestamp());
    }
    ASSERT_EQ(false, result->next());
}

TEST(TestMariaStmt, SetDateTimeStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            MYSQL_CONNECTION_STRING
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto stmt = instance->createStatement("insert into tb_stmt_setTime (id, time) values (?, ?);");
    ASSERT_NE(nullptr, stmt);

    int id = 3;
    auto date_time = sese::DateTime(1679142600000000, 0);
    ASSERT_EQ(true, stmt->setInteger(1, id));
    ASSERT_NE(true, stmt->setInteger(6, id));
    ASSERT_EQ(true, stmt->setDateTime(2, date_time));
    ASSERT_NE(true, stmt->setDateTime(6, date_time));

    auto count = stmt->executeUpdate();
    ASSERT_NE(-1, count);

    //    stmt->setDateTime(2, dateTime);
    //    stmt->setInteger(2, id);
    //    stmt->setDateTime(2, dateTime);

    auto result = instance->executeQuery("select * from tb_stmt_setTime;");
    ASSERT_NE(nullptr, result);
    if (result->next()) {
        ASSERT_EQ(1, result->getInteger(0));
        ASSERT_EQ(1679142600000000, result->getDateTime(1).value().getTimestamp());
    }
    if (result->next()) {
        ASSERT_EQ(2, result->getInteger(0));
        ASSERT_EQ(1679142600000000, result->getDateTime(1).value().getTimestamp());
    }
    if (result->next()) {
        ASSERT_EQ(3, result->getInteger(0));
        ASSERT_EQ(1679142600000000, result->getDateTime(1).value().getTimestamp());
    }
    ASSERT_EQ(false, result->next());
}

TEST(TestMariaStmt, isNullStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            MYSQL_CONNECTION_STRING
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto stmt = instance->createStatement("select * from tb_stmt_isNull;");
    ASSERT_NE(nullptr, stmt);

    auto results = stmt->executeQuery();
    ASSERT_NE(nullptr, results);

    ASSERT_TRUE(results->next());
    EXPECT_FALSE(results->isNull(0));
    EXPECT_FALSE(results->isNull(1));
    EXPECT_FALSE(results->isNull(2));
    EXPECT_FALSE(results->isNull(3));
    EXPECT_FALSE(results->isNull(4));
    EXPECT_FALSE(results->isNull(5));

    ASSERT_TRUE(results->next());
    EXPECT_TRUE(results->isNull(0));
    EXPECT_TRUE(results->isNull(1));
    EXPECT_TRUE(results->isNull(2));
    EXPECT_TRUE(results->isNull(3));
    EXPECT_TRUE(results->isNull(4));
    EXPECT_TRUE(results->isNull(5));
}
