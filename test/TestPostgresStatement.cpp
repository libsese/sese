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
    if (results->next()) {
        ASSERT_EQ(1, results->getInteger(0));
        std::string_view strings = "foo";
        ASSERT_EQ(strings, results->getString(1).data());
    }
    ASSERT_EQ(false, results->next());
}

TEST(TestPostgresStmt, UpdateStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

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
    ASSERT_NE(nullptr, results1);
    if (results1->next()) {
        ASSERT_EQ(1, results1->getInteger(0));
        std::string_view strings = "mike";
        ASSERT_EQ(strings, results1->getString(1).data());
    }
    if (results1->next()) {
        ASSERT_EQ(2, results1->getInteger(0));
    }
    ASSERT_EQ(false, results1->next());

}

TEST(TestPostgresStmt, DeleteStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    int32_t id = 1;
    auto stmt = instance->createStatement("delete from tb_stmt_delete where id = ?;");
    ASSERT_NE(nullptr, stmt);

    ASSERT_EQ(true, stmt->setInteger(1, id));

    auto count = stmt->executeUpdate();
    ASSERT_NE(-1, count);

    auto results = instance->executeQuery("select * from tb_stmt_delete;");
    ASSERT_NE(nullptr, results);
    if (results->next()) {
        ASSERT_EQ(2, results->getInteger(0));
        std::string_view strings = "bar";
        ASSERT_EQ(strings, results->getString(1).data());
    }
    ASSERT_EQ(false, results->next());
}

TEST(TestPostgresStmt, InsertStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

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
    auto result = stmtQuery1->executeQuery();
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

TEST(TestPostgresStmt, GetTimeStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    int32_t id = 1;
    auto stmt = instance->createStatement("select * from tb_stmt_getTime where id = ?;");
    ASSERT_NE(nullptr, stmt);

    ASSERT_EQ(true, stmt->setInteger(1, id));

    auto results = stmt->executeQuery();
    ASSERT_NE(nullptr, results);
    if (results->next()) {
        ASSERT_EQ(1, results->getInteger(0));
        ASSERT_EQ(1679142600000000, results->getDateTime(1).value().getTimestamp());
    }
    if (results->next()) {
        ASSERT_EQ(2, results->getInteger(0));
        ASSERT_EQ(1679142600000000, results->getDateTime(1).value().getTimestamp());
    }
    ASSERT_EQ(false, results->next());
}

TEST(TestPostgresStmt, SetTimeStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);


    auto stmt = instance->createStatement("insert into tb_stmt_setTime (id, time) values (?, ?);");
    ASSERT_NE(nullptr, stmt);

    int32_t id = 3;
    auto dateTime = sese::DateTime(1679142600000000, 0);
    ASSERT_EQ(true, stmt->setInteger(1, id));
    ASSERT_EQ(true, stmt->setDateTime(2, dateTime));

    auto count = stmt->executeUpdate();
    ASSERT_NE(-1, count);

    auto results = instance->executeQuery("select * from tb_stmt_setTime;");
    ASSERT_NE(nullptr, results);
    if (results->next()) {
        ASSERT_EQ(1, results->getInteger(0));
        ASSERT_EQ(1679142600000000, results->getDateTime(1).value().getTimestamp());
    }
    if (results->next()) {
        ASSERT_EQ(2, results->getInteger(0));
        ASSERT_EQ(1679142600000000, results->getDateTime(1).value().getTimestamp());
    }
    if (results->next()) {
        ASSERT_EQ(3, results->getInteger(0));
        ASSERT_EQ(1679142600000000, results->getDateTime(1).value().getTimestamp());
    }
    ASSERT_EQ(false, results->next());
}

TEST(TestPostgresStmt, isNullStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    auto stmt = instance->createStatement("select * from tb_stmt_isNull;");
    ASSERT_NE(nullptr, stmt);

    auto results = stmt->executeQuery();
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

    auto stmt1 = instance->createStatement("update tb_stmt_isNull set name = ?, doubleNull = ?, floatNull = ?, longNull = ?, dateTimeNull = ? where id = ?;");
    ASSERT_NE(nullptr, stmt1);

    int32_t id = 1;
    ASSERT_EQ(true, stmt1->setNull(1));
    ASSERT_EQ(true, stmt1->setNull(2));
    ASSERT_EQ(true, stmt1->setNull(3));
    ASSERT_EQ(true, stmt1->setNull(4));
    ASSERT_EQ(true, stmt1->setNull(5));
    ASSERT_EQ(true, stmt1->setInteger(6, id));

    auto count = stmt1->executeUpdate();
    ASSERT_NE(-1, count);

    auto stmt2 = instance->createStatement("update tb_stmt_isNull set id = ?, doubleNull = ?, floatNull = ?, longNull = ?, dateTimeNull = ? where name = ?;");
    ASSERT_NE(nullptr, stmt2);

    const char *name = "bar";
    ASSERT_EQ(true, stmt2->setNull(1));
    ASSERT_EQ(true, stmt2->setNull(2));
    ASSERT_EQ(true, stmt2->setNull(3));
    ASSERT_EQ(true, stmt2->setNull(4));
    ASSERT_EQ(true, stmt2->setNull(5));
    ASSERT_EQ(true, stmt2->setText(6, name));

    auto count1 = stmt2->executeUpdate();
    ASSERT_NE(-1, count1);

    auto stmt3 = instance->createStatement("select * from tb_stmt_isNull");
    ASSERT_NE(nullptr, stmt3);

    auto results1 = stmt3->executeQuery();
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