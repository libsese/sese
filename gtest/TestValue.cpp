#include <sese/util/Value.h>

#include <gtest/gtest.h>

using sese::Value;

TEST(TestValue, IdentityNull) {
    Value value;
    EXPECT_TRUE(value.isNull());
    EXPECT_FALSE(value.isBool());
    EXPECT_FALSE(value.isInt());
    EXPECT_FALSE(value.isDouble());
}

TEST(TestValue, IdentityBool) {
    Value value(true);
    EXPECT_FALSE(value.isNull());
    EXPECT_TRUE(value.isBool());
    EXPECT_FALSE(value.isInt());
    EXPECT_FALSE(value.isDouble());

    EXPECT_EQ(value.getIfBool().value(), true);
    EXPECT_FALSE(value.getIfInt().has_value());

    EXPECT_EQ(value.getBool(), true);
}

TEST(TestValue, IdentityInt) {
    Value value(1);
    EXPECT_FALSE(value.isNull());
    EXPECT_FALSE(value.isBool());
    EXPECT_TRUE(value.isInt());
    EXPECT_FALSE(value.isDouble());

    EXPECT_EQ(value.getIfInt().value(), 1);

    EXPECT_EQ(value.getInt(), 1);
}

TEST(TestValue, IdentityDouble) {
    Value value(1.0);
    EXPECT_FALSE(value.isNull());
    EXPECT_FALSE(value.isBool());
    EXPECT_FALSE(value.isInt());
    EXPECT_TRUE(value.isDouble());

    EXPECT_EQ(value.getIfDouble().value(), 1.0);

    EXPECT_EQ(value.getDouble(), 1.0);
}

TEST(TestValue, IdentityString) {
    Value value("test");
    EXPECT_FALSE(value.isNull());
    EXPECT_FALSE(value.isBool());
    EXPECT_FALSE(value.isInt());
    EXPECT_FALSE(value.isDouble());
    EXPECT_TRUE(value.isString());

    EXPECT_EQ(*value.getIfString(), "test");

    EXPECT_EQ(value.getString(), "test");
}

TEST(TestValue, IdentityBlob) {
    Value value("test", 4);
    EXPECT_FALSE(value.isNull());
    EXPECT_FALSE(value.isBool());
    EXPECT_FALSE(value.isInt());
    EXPECT_FALSE(value.isDouble());
    EXPECT_TRUE(value.isBlob());

    EXPECT_TRUE(value.getIfBlob());

    auto data = value.getBlob();
    EXPECT_EQ(memcmp(data.data(), "test", data.size()), 0);
}

TEST(TestValue, IdentityList) {
    auto value = Value::list();
    EXPECT_FALSE(value.isNull());
    EXPECT_FALSE(value.isBool());
    EXPECT_FALSE(value.isInt());
    EXPECT_FALSE(value.isDouble());
    EXPECT_TRUE(value.isList());
}

TEST(TestValue, IdentityDict) {
    auto value = Value::dict();
    EXPECT_FALSE(value.isNull());
    EXPECT_FALSE(value.isBool());
    EXPECT_FALSE(value.isInt());
    EXPECT_FALSE(value.isDouble());
    EXPECT_TRUE(value.isDict());
}

TEST(TestValue, ConvertInt) {
    Value value(1);
    EXPECT_EQ(value.getIfInt().value(), 1);
    EXPECT_EQ(value.getIfBool().value(), true);
    EXPECT_EQ(value.getIfDouble().value(), 1.0);

    value = Value(0);
    EXPECT_EQ(value.getIfInt().value(), 0);
    EXPECT_EQ(value.getIfBool().value(), false);
    EXPECT_EQ(value.getIfDouble().value(), 0.0);
}

TEST(TestValue, ConvertDouble) {
    Value value(1.0);
    EXPECT_EQ(value.getIfInt().value(), 1);
    EXPECT_EQ(value.getIfBool().value(), true);
    EXPECT_EQ(value.getIfDouble().value(), 1.0);

    value = Value(0.0);
    EXPECT_EQ(value.getIfInt().value(), 0);
    EXPECT_EQ(value.getIfBool().value(), false);
    EXPECT_EQ(value.getIfDouble().value(), 0.0);
}

TEST(TestValue, CompareBool) {
    Value value1(true);
    Value value2(true);
    EXPECT_EQ(value1, value2);

    Value value3(false);
    EXPECT_NE(value1, value3);
}

TEST(TestValue,CompareInt) {
    Value value1(1);
    Value value2(1);
    EXPECT_EQ(value1, value2);

    Value value3(2);
    EXPECT_NE(value1, value3);
}

TEST(TestValue, CompareString) {
    Value value1("test");
    Value value2("test");
    EXPECT_EQ(value1, value2);

    Value value3("test2");
    EXPECT_NE(value1, value3);
}

TEST(TestValue, CompareBlob) {
    Value value1("test", 4);
    Value value2("test", 4);
    EXPECT_NE(value1, value2);
}

TEST(TestValue, CompareList) {
    Value value1(Value::List().append(1).append(2));
    Value value2(Value::List().append(1).append(2));
    EXPECT_NE(value1, value2);
}

TEST(TestValue, CompareDict) {
    Value value1(Value::Dict().set("a", 1).set("b", 2));
    Value value2(Value::Dict().set("a", 1).set("b", 2));
    EXPECT_NE(value1, value2);
}