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
    ASSERT_NE(nullptr, instance) << "Failed to create the database object instance. Procedure";
}

TEST(TestDriverInstance, TestOperationalData) {
    // 改
    auto count = instance->executeUpdate("update tb_user set name = 'shiina_kaoru' where id = 1;");
    EXPECT_NE(-1, count) << "Update failure";
    // 查某行数据
    auto result = instance->executeQuery("select id from tb_user where id = 1");
    EXPECT_NE(nullptr, result) << "Query failure";
    // 增
    auto count2 = instance->executeUpdate("insert into tb_user (id, name) values(2, 'suzuran')");
    EXPECT_NE(-1, count2) << "Update failure";
    // 删
    auto count3 = instance->executeUpdate("delete from tb_user where id = 1");
    EXPECT_NE(-1, count3) << "Update failure";
    // 查整个表
    auto result1 = instance->executeQuery("select * from tb_user;");
    EXPECT_NE(nullptr, result1) << "Query failure";

}

TEST(TestDriverInstance, TestCreateStmt){

        int64_t value = 1;
        const char *table = "tb_user";
        auto stmt = instance->createStatement("select name from tb_user where id = ?;");
        ASSERT_NE(nullptr,stmt)<<"Creation failure";

        EXPECT_EQ(true, stmt->setLong(1, value));

        auto result = stmt->executeQuery();
        EXPECT_NE(nullptr, result) << "Query failure";

        int64_t id = 9;
        const char *name = "sese-db";
        auto stmt1 = instance->createStatement("update tb_user set name = ? where id = ?");
        ASSERT_NE(nullptr,stmt1)<<"Creation failure";

        EXPECT_EQ(true, stmt1->setText(1, name));
        EXPECT_EQ(true, stmt1->setLong(2, id));

        auto count = stmt1->executeUpdate();
        EXPECT_NE(-1, count) << "Update failure";
}