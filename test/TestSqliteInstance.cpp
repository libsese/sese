#include <sese/db/DriverManager.h>

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

int main() {
    auto instance = DriverManager::getInstance(DatabaseType::Sqlite, "db_test.db");
    auto result = instance->executeQuery("select user.id, user.name from tb_user user where id = 1;");
    printf("id = %s, name = %s\n", result->get(1, 0), result->get(1, 1));
    auto count = instance->executeUpdate(R"(update tb_user set name = "kaoru" where id = 1;)");
    printf("update rows: %d\n", (int) count);
    return 0;
}