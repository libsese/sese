#include <sese/db/DriverManager.h>

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

int main() {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, "db_test.db");
    {
        auto result = instance->executeQuery("select user.id, user.name from tb_user user where id = 1;");
        result->next();
        printf("id = %s, name = %s\n", result->getColumnByIndex(0), result->getColumnByIndex(1));
        auto count = instance->executeUpdate(R"(update tb_user set name = "kaoru" where id = 1;)");
        printf("update rows: %d\n", (int) count);
    }

    {
        auto stmt = instance->createStatement("update tb_user set name = ? where id = ?;");
        stmt->setText(1, "mike");
        stmt->setInteger(2, 2);
        auto count = stmt->executeUpdate();
        printf("stmt update rows: %d\n", (int) count);
    }

    {
        auto stmt = instance->createStatement("select * from tb_user;");
        auto result = stmt->executeQuery();
        while (result->next()) {
            printf("stmt result: id = %s, name = %s\n", result->getColumnByIndex(0), result->getColumnByIndex(1));
        }
    }

    return 0;
}