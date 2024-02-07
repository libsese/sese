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

    int64_t id = 1;
    const char *name = "mike";
    auto stmt = instance->createStatement("update tb_stmt_update set name = ? where id = ?;");
    ASSERT_NE(nullptr, stmt);

    ASSERT_EQ(true, stmt->setText(1, name));
    ASSERT_EQ(true, stmt->setLong(2, id));

    auto count = stmt->executeUpdate();
    ASSERT_NE(-1, count);

    auto result = instance->executeQuery("select * from tb_stmt_update where id = 1;");
    if (result->next()) {
        ASSERT_EQ(1, result->getInteger(0));
        std::string_view strings = "mike";
        ASSERT_EQ(strings, result->getString(1).data());
    }
    ASSERT_EQ(false, result->next());

    id = 2;
    ASSERT_EQ(true, stmt->setNull(1));
    ASSERT_EQ(true, stmt->setLong(2, id));

    auto count1 = stmt->executeUpdate();
    ASSERT_NE(-1, count1);

    auto result1 = instance->executeQuery("select * from tb_stmt_update where id = 2;");
    if (result1->next()) {
        ASSERT_EQ(2, result1->getInteger(0));
    }
    ASSERT_EQ(false, result1->next());
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

TEST(TestSqliteStmt, Deletestmt) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    int64_t id = 1;
    auto stmt = instance->createStatement("delete from tb_stmt_delete where id = ?;");
    ASSERT_NE(nullptr, stmt);

    ASSERT_EQ(true, stmt->setLong(1, id));

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

TEST(TestSqliteStmt, Insertstmt) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

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

//    ASSERT_EQ(true, stmt->setInteger(1, id));
//    ASSERT_EQ(true, stmt->setText(2, name));
//    ASSERT_EQ(true, stmt->setDouble(3, dou));
//    ASSERT_EQ(true, stmt->setFloat(4, flo));
//    ASSERT_EQ(true, stmt->setLong(5, lon));
//    ASSERT_EQ(true, stmt->setNull(6));
//
//    auto count1 = stmt->executeUpdate();
//    ASSERT_NE(-1, count1);
//
//    ASSERT_EQ(true, stmt->setText(2, name));
//    ASSERT_EQ(true, stmt->setInteger(1, id));
//    ASSERT_EQ(true, stmt->setDouble(3, dou));
//    ASSERT_EQ(true, stmt->setFloat(4, flo));
//    ASSERT_EQ(true, stmt->setLong(5, lon));
//    ASSERT_EQ(true, stmt->setNull(6));
//
//    auto count2 = stmt->executeUpdate();
//    ASSERT_NE(-1, count2);
//
//    ASSERT_EQ(true, stmt->setDouble(3, dou));
//    ASSERT_EQ(true, stmt->setText(2, name));
//    ASSERT_EQ(true, stmt->setInteger(1, id));
//    ASSERT_EQ(true, stmt->setFloat(4, flo));
//    ASSERT_EQ(true, stmt->setLong(5, lon));
//    ASSERT_EQ(true, stmt->setNull(6));
//
//    auto count3 = stmt->executeUpdate();
//    ASSERT_NE(-1, count3);
//
//    ASSERT_EQ(true, stmt->setFloat(4, flo));
//    ASSERT_EQ(true, stmt->setDouble(3, dou));
//    ASSERT_EQ(true, stmt->setText(2, name));
//    ASSERT_EQ(true, stmt->setInteger(1, id));
//    ASSERT_EQ(true, stmt->setLong(5, lon));
//    ASSERT_EQ(true, stmt->setNull(6));
//
//    auto count4 = stmt->executeUpdate();
//    ASSERT_NE(-1, count4);
//
//    ASSERT_EQ(true, stmt->setLong(5, lon));
//    ASSERT_EQ(true, stmt->setFloat(4, flo));
//    ASSERT_EQ(true, stmt->setDouble(3, dou));
//    ASSERT_EQ(true, stmt->setText(2, name));
//    ASSERT_EQ(true, stmt->setInteger(1, id));
//    ASSERT_EQ(true, stmt->setNull(6));


    auto stmtQuery = instance->createStatement("select id, name, setDouble, setFloat, setLong from tb_stmt_insert;");
    ASSERT_NE(nullptr, stmtQuery);

    auto result = stmtQuery->executeQuery();
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
        ASSERT_EQ(3, result->getLong(4));
    }
    ASSERT_EQ(false, result->next());
}

TEST(TestSqliteStmt, GetTimeStmt) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
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

TEST(TestSqliteStmt, SetTimeStmt) {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, PATH_TO_DB);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto stmt = instance->createStatement("insert into tb_stmt_setTime (id, intTime, charTime) values (?, ?, ?);");
    ASSERT_NE(nullptr, stmt);

    int id = 1;
    auto dateTime = sese::DateTime(1679142600000000, 0);

    ASSERT_EQ(true, stmt->setInteger(1, id));
//    ASSERT_NE(true, stmt->setInteger(6, id));
    ASSERT_EQ(true, stmt->setDateTime(2, dateTime));
//    ASSERT_NE(true, stmt->setDateTime(6, dateTime));
    ASSERT_EQ(true, stmt->setDateTime(3, dateTime));
//    ASSERT_NE(true, stmt->setDateTime(6, dateTime));


    auto count = stmt->executeUpdate();
    ASSERT_NE(-1, count);

    auto result = instance->executeQuery("select * from tb_stmt_setTime where id = 3;");
    ASSERT_NE(nullptr, result);

    if (result->next()) {
        ASSERT_EQ(1, result->getInteger(0));
        ASSERT_EQ(1679142600000000, result->getDateTime(1).value().getTimestamp());
        ASSERT_EQ(1679142600000000, result->getDateTime(2).value().getTimestamp());
    }
    ASSERT_EQ(false, result->next());
}

TEST(TestMariaStmt, isNullStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=libsese;db=db_test;port=18806;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(0, instance->getLastError());

    auto stmt = instance->createStatement("update tb_stmt_isNull set name = ?, doubleNull = ?, floatNull = ?, longNull = ?, dateTimeNull = ? where id = ?;");
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

    auto stmt1 = instance->createStatement("update tb_stmt_isNull set id = ?, doubleNull = ?, floatNull = ?, longNull = ?, dateTimeNull = ? where name = ?;");
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

    auto stmt2 = instance->createStatement("select * from tb_stmt_isNull");
    ASSERT_NE(nullptr, stmt2);

    auto results = stmt2->executeQuery();
    ASSERT_NE(nullptr, results);

    if (results->next()) {
        ASSERT_EQ(1, results->getInteger(0));
        ASSERT_EQ(true, results->isNull(1));
        ASSERT_EQ(true, results->isNull(2));
        ASSERT_EQ(true, results->isNull(3));
        ASSERT_EQ(true, results->isNull(4));
        ASSERT_EQ(true, results->isNull(5));
    }
    if (results->next()) {
        ASSERT_EQ(true, results->isNull(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, results->getString(1));
        ASSERT_EQ(true, results->isNull(2));
        ASSERT_EQ(true, results->isNull(3));
        ASSERT_EQ(true, results->isNull(4));
        ASSERT_EQ(true, results->isNull(5));
    }
    ASSERT_EQ(false, results->next());
}