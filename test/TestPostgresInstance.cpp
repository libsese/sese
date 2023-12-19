#include <sese/db/DriverManager.h>
#include <gtest/gtest.h>

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

// instanceError
TEST(TestDriverInstance, TestInstanceError) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=error;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_NE(instance->getLastError(), 0);
    printf("errorMsg:%s\n", instance->getLastErrorMessage());

    auto results = instance->executeQuery("select * from tb_query;");

    auto results1 = instance->executeUpdate("update tb_update set name = 'mike' where id = 1;");

    auto stmt = instance->createStatement("select * from tb_query;");
    auto results2 = stmt->executeQuery();

    auto stmt2 = instance->createStatement("update tb_update set name = 'mike' where id = 1;");
    auto results3 = stmt->executeUpdate();

    auto results4 = instance->commit();

    auto results5 = instance->rollback();

    auto results6 = instance->begin();
}

// query
TEST(TestDriverInstance, TestQueryData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);
    printf("errorMsg:%s\n", instance->getLastErrorMessage());

    // error
    auto results = instance->executeQuery("select * from tb_query_error;");
    ASSERT_EQ(nullptr, results);
    ASSERT_NE(instance->getLastError(), 0);
    printf("errorMsg: %s\n", instance->getLastErrorMessage());

    auto results1 = instance->executeQuery("select * from tb_query;");
    ASSERT_NE(nullptr, results1);
    printf("Columns = %zu", results1->getColumns());
    while (results1->next()) {
        printf("id = %d, name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }

    results1->reset();
    while (results1->next()) {
        printf("id = %d, name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }

    results1 = instance->executeQuery("select * from tb_query where id = 1;");
    ASSERT_NE(nullptr, results1);
    while (results1->next()) {
        printf("id = %d, name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }
}

// update
TEST(TestDriverInstance, TestUpdateData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    auto results = instance->executeQuery("select * from tb_update;");
    while (results->next()) {
        printf("id = %d, name = %s\n", (int) results->getInteger(0), results->getString(1).data());
    }

    auto count = instance->executeUpdate("update tb_update set name = 'mike' where id = 1;");
    ASSERT_NE(-1, count);

    count = instance->executeUpdate("update tb_update set name = 'bar' where id = 2;");
    ASSERT_NE(-1, count);

    // error
    auto count1 = instance->executeUpdate("update tb_update_error set name = 'mike' where id = 1;");
    ASSERT_EQ(-1, count1);
    ASSERT_NE(instance->getLastError(), 0);
    printf("errorMsg: %s\n", instance->getLastErrorMessage());

    auto results1 = instance->executeQuery("select * from tb_update;");
    while (results1->next()) {
        printf("id = %d, name = %s\n", (int) results1->getInteger(0), results1->getString(1).data());
    }
}

// delete
TEST(TestDriverInstance, TestDeleteData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    auto results = instance->executeQuery("select * from tb_delete;");
    while (results->next()) {
        printf("id = %d, name = %s\n", (int) results->getInteger(0), results->getString(1).data());
    }

    auto count = instance->executeUpdate("delete from tb_delete where id = 1;");
    ASSERT_NE(-1, count);

    auto results1 = instance->executeQuery("select * from tb_delete;");
    ASSERT_NE(nullptr, results1);
    while (results1->next()) {
        printf("id = %d, name = %s\n", (int) results1->getInteger(0), results1->getString(1).data());
    }
}

// insert
TEST(TestDriverInstance, TestInsertData) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    auto results = instance->executeQuery("select * from tb_insert;");
    ASSERT_NE(nullptr, results);
    while (results->next()) {
        printf("id = %d, name = %s\n", (int) results->getInteger(0), results->getString(1).data());
    }

    auto count = instance->executeUpdate("insert into tb_insert (id, name) values (3,'mike');");
    ASSERT_NE(-1, count);

    auto results1 = instance->executeQuery("select * from tb_insert;");
    ASSERT_NE(nullptr, results1);
    while (results1->next()) {
        printf("id = %d, name = %s\n", (int) results1->getInteger(0), results1->getString(1).data());
    }
}

// create select_stmt
TEST(TestDriverInstance, TestQueryStmt) {
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
        printf("id = %d, name = %s\n", (int) results->getInteger(0), results->getString(1).data());
    }
}

// create update_stmt
TEST(TestDriverInstance, TestUpdateStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    auto results = instance->executeQuery("select * from tb_stmt_update;");
    ASSERT_NE(nullptr, results);
    while (results->next()) {
        printf("id = %d, name = %s\n", (int) results->getInteger(0), results->getString(1).data());
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
        printf("id = %d, name = %s\n", (int) results1->getInteger(0), results1->getString(1).data());
    }

}

// create delete_stmt
TEST(TestDriverInstance, TestDeleteStmt) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    auto results = instance->executeQuery("select * from tb_stmt_delete;");
    ASSERT_NE(nullptr, results);
    while (results->next()) {
        printf("id = %d, name = %s\n", (int) results->getInteger(0), results->getString(1).data());
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
        printf("id = %d, name = %s\n", (int) results1->getInteger(0), results1->getString(1).data());
    }
}

// create insert_stmt
TEST(TestDriverInstance, TestInsertStmt) {
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
        printf("result: id = %d name = %s setDouble = %lf setFloat = %f setInteger = %d\n", (int) result->getLong(0), result->getString(1).data(), result->getDouble(2), result->getFloat(3), result->getInteger(4));
    }

    int64_t id = 3;
    const char *name = "mike";
    int32_t integer = 78;
    float flo = 5.1;
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
    EXPECT_NE(-1, count);

    auto stmtQuery1 = instance->createStatement("select * from tb_stmt_insert;");
    auto result1 = stmtQuery1->executeQuery();
    ASSERT_NE(nullptr, result1);
    while (result1->next()) {
        printf("result1: id = %d name = %s setDouble = %lf setFloat = %f setInteger = %d\n", (int) result1->getLong(0), result1->getString(1).data(), result1->getDouble(2), result1->getFloat(3), result1->getInteger(4));
    }
}

// commit
TEST(TestTransaction, TestCommit) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    auto results = instance->executeQuery("select * from tb_commit;");
    while (results->next()) {
        printf("id = %d, name = %s\n", results->getInteger(0), results->getString(1).data());
    }

    ASSERT_EQ(true, instance->begin());

    auto count = instance->executeUpdate("insert into tb_commit (id, name) values (3, 'mike')");
    ASSERT_NE(-1, count);

    ASSERT_EQ(true, instance->commit());

    auto results1 = instance->executeQuery("select * from tb_commit;");
    while (results1->next()) {
        printf("id = %d, name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }
}

// rollBack
TEST(TestTransaction, TestRollBack) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    ASSERT_EQ(true, instance->begin());

    auto results = instance->executeQuery("select * from tb_rollBack;");
    while (results->next()) {
        printf("id = %d, name = %s\n", results->getInteger(0), results->getString(1).data());
    }

    auto count = instance->executeUpdate("insert into tb_rollBack (id, name) values (3, 'mike');");
    ASSERT_NE(-1, count);

    ASSERT_EQ(true, instance->rollback());

    auto results1 = instance->executeQuery("select * from tb_rollBack;");
    while (results1->next()) {
        printf("id = %d, name = %s\n", results1->getInteger(0), results1->getString(1).data());
    }
}

// begin
TEST(TestTransaction, TestBegin) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    // 事务的回滚操作
    auto results = instance->executeQuery("select * from tb_begin;");
    while (results->next()) {
        printf("id = %d, name = %s\n", results->getInteger(0), results->getString(1).data());
    }

    ASSERT_EQ(true, instance->begin());

    auto count = instance->executeUpdate("insert into tb_begin (id, name) values (3, 'mike');");
    ASSERT_NE(-1, count);

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
    ASSERT_NE(-1, count1);

    ASSERT_EQ(true, instance->commit());

    auto results3 = instance->executeQuery("select * from tb_begin;");
    while (results3->next()) {
        printf("id = %d, name = %s\n", results3->getInteger(0), results3->getString(1).data());
    }
}

TEST(TestTransaction, UseLess) {
    auto instance = DriverManager::getInstance(
            DatabaseType::Postgres,
            "host=127.0.0.1;user=postgres;pwd=libsese;db=db_test;port=18080;"
    );
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(instance->getLastError(), 0);

    auto stmt = instance->createStatement("select * from tb_query_error;");
    auto results1 = stmt->executeQuery();

    auto stmt2 = instance->createStatement("update tb_update set name = 'mike' where id = 1;");
    auto results2 = stmt->executeUpdate();

    auto a = instance->setAutoCommit(true);

    bool b1;
    auto b = instance->getAutoCommit(b1);

    int64_t c1;
    auto c = instance->getInsertId(c1);
}
