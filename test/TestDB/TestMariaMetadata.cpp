#include <gtest/gtest.h>
#include <sese/db/DriverManager.h>

#include "Config.h"

class TestMariaMetadata : public testing::Test {
protected:
    static sese::db::DriverInstance::Ptr instance;

    static void SetUpTestSuite() {
        instance = sese::db::DriverManager::getInstance(sese::db::DatabaseType::MY_SQL, MYSQL_CONNECTION_STRING);
        ASSERT_NE(nullptr, instance);
    }
};

sese::db::DriverInstance::Ptr TestMariaMetadata::instance;

TEST_F(TestMariaMetadata, CheckType) {
    auto stmt = instance->createStatement("select * from tb_metadata;");
    ASSERT_NE(nullptr, stmt);
    {
        sese::db::MetadataType type;
        ASSERT_TRUE(stmt->getColumnType(0, type));
        EXPECT_EQ(sese::db::MetadataType::INTEGER, type);
    }
    {
        sese::db::MetadataType type;
        ASSERT_TRUE(stmt->getColumnType(1, type));
        EXPECT_EQ(sese::db::MetadataType::TEXT, type);
    }
    {
        sese::db::MetadataType type;
        ASSERT_TRUE(stmt->getColumnType(2, type));
        EXPECT_EQ(sese::db::MetadataType::FLOAT, type);
    }
    {
        sese::db::MetadataType type;
        ASSERT_TRUE(stmt->getColumnType(3, type));
        EXPECT_EQ(sese::db::MetadataType::DOUBLE, type);
    }
    {
        sese::db::MetadataType type;
        ASSERT_TRUE(stmt->getColumnType(4, type));
        EXPECT_EQ(sese::db::MetadataType::UNKNOWN, type);
    }
    {
        sese::db::MetadataType type;
        ASSERT_FALSE(stmt->getColumnType(5, type));
    }
}

TEST_F(TestMariaMetadata, CheckSize) {
    auto stmt = instance->createStatement("select * from tb_metadata;");
    ASSERT_NE(nullptr, stmt);
    EXPECT_EQ(stmt->getColumnSize(0), 11);    // 默认显示位
    EXPECT_EQ(stmt->getColumnSize(1), 31 * 4);// 字符集为 utf8mb4
    EXPECT_EQ(stmt->getColumnSize(2), 12);    // 默认显示位
    EXPECT_EQ(stmt->getColumnSize(3), 22);    // 默认显示位
    EXPECT_EQ(stmt->getColumnSize(4), 255);   // 二进制向上取整
}