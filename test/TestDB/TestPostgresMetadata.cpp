#include <gtest/gtest.h>
#include <sese/db/DriverManager.h>

#include "Config.h"

class TestPostgresMetadata : public testing::Test {
protected:
    static sese::db::DriverInstance::Ptr instance;

    static void SetUpTestSuite() {
        instance = sese::db::DriverManager::getInstance(sese::db::DatabaseType::POSTGRES, PSQL_CONNECTION_STRING);
        ASSERT_NE(nullptr, instance);
    }
};

sese::db::DriverInstance::Ptr TestPostgresMetadata::instance;

TEST_F(TestPostgresMetadata, CheckType) {
    auto stmt = instance->createStatement("select * from tb_metadata;");
    ASSERT_NE(nullptr, instance);
    {
        sese::db::MetadataType type;
        ASSERT_TRUE(stmt->getColumnType(0, type));
        EXPECT_EQ(sese::db::MetadataType::SHORT, type);
    }
    {
        sese::db::MetadataType type;
        ASSERT_TRUE(stmt->getColumnType(1, type));
        EXPECT_EQ(sese::db::MetadataType::INTEGER, type);
    }
    {
        sese::db::MetadataType type;
        ASSERT_TRUE(stmt->getColumnType(2, type));
        EXPECT_EQ(sese::db::MetadataType::LONG, type);
    }
    {
        sese::db::MetadataType type;
        ASSERT_TRUE(stmt->getColumnType(3, type));
        EXPECT_EQ(sese::db::MetadataType::BOOLEAN, type);
    }
    {
        sese::db::MetadataType type;
        ASSERT_TRUE(stmt->getColumnType(4, type));
        EXPECT_EQ(sese::db::MetadataType::TEXT, type);
    }
    {
        sese::db::MetadataType type;
        ASSERT_TRUE(stmt->getColumnType(5, type));
        EXPECT_EQ(sese::db::MetadataType::FLOAT, type);
    }
    {
        sese::db::MetadataType type;
        ASSERT_TRUE(stmt->getColumnType(6, type));
        EXPECT_EQ(sese::db::MetadataType::UNKNOWN, type);
    }
}