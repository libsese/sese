#include <sese/db/DriverManager.h>
#include <sese/util/Initializer.h>
#include <gtest/gtest.h>
#include <cinttypes>

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

TEST(TestSqliteDriverInstance, Error) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto resultError = instance->executeQuery("select * from tb_query_error where id = 1;");
    ASSERT_EQ(nullptr, resultError);

    ASSERT_NE(nullptr, instance->getLastErrorMessage());

    //stmtError
    auto resultStmtError = instance->createStatement("select * from tb_stmt_query_error where id = 1;");
    ASSERT_EQ(nullptr, resultStmtError);

    ASSERT_NE(nullptr, instance->getLastErrorMessage());
}

TEST(TestSqliteDriverInstance, QueryData) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_query;");
    ASSERT_NE(nullptr, result);
    ASSERT_EQ(2, result->getColumns());
    if (result->next()) {
        ASSERT_EQ(1, result->getInteger(0));
        std::string_view strings = "foo";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    if (result->next()) {
        ASSERT_EQ(2, result->getInteger(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    ASSERT_EQ(false, result->next());

    result->reset();

    if (result->next()) {
        ASSERT_EQ(1, result->getInteger(0));
        std::string_view strings = "foo";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    if (result->next()) {
        ASSERT_EQ(2, result->getInteger(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    ASSERT_EQ(false, result->next());
}

TEST(TestSqliteDriverInstance, UpdateData) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto count = instance->executeUpdate(R"(update tb_update set name = "mike" where id = 1;)");
    ASSERT_NE(-1, count) << instance->getLastErrorMessage();

    auto result = instance->executeQuery("select * from tb_update where id = 1;");
    ASSERT_NE(nullptr, result);
    if (result->next()) {
        ASSERT_EQ(1, result->getInteger(0));
        std::string_view strings = "mike";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    ASSERT_EQ(false, result->next());
}

TEST(TestSqliteDriverInstance, DeleteData) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto count = instance->executeUpdate("delete from tb_delete where id = 1;");
    ASSERT_NE(-1, count);

    auto result = instance->executeQuery("select * from tb_delete;");
    ASSERT_NE(nullptr, result);
    if (result->next()) {
        ASSERT_EQ(2, result->getInteger(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    ASSERT_EQ(false, result->next());
}

TEST(TestSqliteDriverInstance, InsertData) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto count = instance->executeUpdate("insert into tb_insert (id, name) values (3, 'mike');");
    ASSERT_NE(-1, count);

    auto result = instance->executeQuery("select * from tb_insert;");
    ASSERT_NE(nullptr, result);
    if (result->next()) {
        ASSERT_EQ(1, result->getInteger(0));
        std::string_view strings = "foo";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    if (result->next()) {
        ASSERT_EQ(2, result->getInteger(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    if (result->next()) {
        ASSERT_EQ(3, result->getInteger(0));
        std::string_view strings = "mike";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    ASSERT_EQ(false, result->next());
}

TEST(TestSqliteDriverInstance, DateTime) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto result = instance->executeQuery("select * from tb_dateTime;");
    ASSERT_NE(nullptr, result);
    if (result->next()) {
        ASSERT_EQ(1, result->getInteger(0));
        ASSERT_EQ(1679142600000000, result->getDateTime(1).value().getTimestamp());
    }
    if (result->next()) {
        ASSERT_EQ(2, result->getInteger(0));
        ASSERT_EQ(1679142600000000, result->getDateTime(1).value().getTimestamp());
    }
}

TEST(TestSqliteDriverInstance, isNull) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto results = instance->executeQuery("select * from tb_isNull");
    ASSERT_NE(nullptr, results);

    if (results->next()) {
        ASSERT_EQ(false, results->isNull(0));
        ASSERT_EQ(false, results->isNull(1));
        ASSERT_EQ(false, results->isNull(2));
        ASSERT_EQ(false, results->isNull(3));
        ASSERT_EQ(false, results->isNull(4));
        ASSERT_EQ(false, results->isNull(5));
    }
    if (results->next()) {
        ASSERT_EQ(false, results->isNull(0));
        ASSERT_EQ(false, results->isNull(1));
        ASSERT_EQ(false, results->isNull(2));
        ASSERT_EQ(false, results->isNull(3));
        ASSERT_EQ(false, results->isNull(4));
        ASSERT_EQ(false, results->isNull(5));
    }
    ASSERT_EQ(false, results->next());

    auto stmt = instance->createStatement("update tb_isNull set name = ?, doubleNull = ?, floatNull = ?, longNull = ?, dateTimeNull = ? where id = ?;");
    ASSERT_NE(nullptr, stmt);

    int32_t id = 1;
    ASSERT_EQ(true, stmt->setNull(1));
    ASSERT_EQ(true, stmt->setNull(2));
    ASSERT_EQ(true, stmt->setNull(3));
    ASSERT_EQ(true, stmt->setNull(4));
    ASSERT_EQ(true, stmt->setNull(5));
    ASSERT_EQ(true, stmt->setInteger(6, id));

    auto count = stmt->executeUpdate();
    ASSERT_NE(-1, count);

    auto stmt1 = instance->createStatement("update tb_isNull set id = ?, doubleNull = ?, floatNull = ?, longNull = ?, dateTimeNull = ? where name = ?;");
    ASSERT_NE(nullptr, stmt1);

    const char *name = "bar";
    ASSERT_EQ(true, stmt1->setNull(1));
    ASSERT_EQ(true, stmt1->setNull(2));
    ASSERT_EQ(true, stmt1->setNull(3));
    ASSERT_EQ(true, stmt1->setNull(4));
    ASSERT_EQ(true, stmt1->setNull(5));
    ASSERT_EQ(true, stmt1->setText(6, name));

    auto count1 = stmt1->executeUpdate();
    ASSERT_NE(-1, count1);

    auto results1 = instance->executeQuery("select * from tb_isNull");
    ASSERT_NE(nullptr, results1);

    if (results1->next()) {
        ASSERT_EQ(1, results1->getInteger(0));
        ASSERT_EQ(true, results1->isNull(1));
        ASSERT_EQ(true, results1->isNull(2));
        ASSERT_EQ(true, results1->isNull(3));
        ASSERT_EQ(true, results1->isNull(4));
        ASSERT_EQ(true, results1->isNull(5));
    }
    if (results1->next()) {
        ASSERT_EQ(true, results1->isNull(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, results1->getString(1));
        ASSERT_EQ(true, results1->isNull(2));
        ASSERT_EQ(true, results1->isNull(3));
        ASSERT_EQ(true, results1->isNull(4));
        ASSERT_EQ(true, results1->isNull(5));
    }
    ASSERT_EQ(false, results1->next());
}