#include <sese/db/DriverManager.h>

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

int main() {
    auto instance = DriverManager::getInstance(
            DatabaseType::Maria,
            "host=127.0.0.1;user=root;pwd=2001;db=db_test;port=3306;"
    );

//    {
//        auto count = instance->executeUpdate("update tb_user set name = 'shiina_kaoru' where id = 1;");
//        printf("update rows: %d\n", (int) count);
//
//        auto result = instance->executeQuery("select * from tb_user;");
//        while (result->next()) {
//            printf("id = %s, name = %s\n", result->getColumnByIndex(0), result->getColumnByIndex(1));
//        }
//    }

    {
        int64_t value = 1;
        auto stmt = instance->createStatement("select * from tb_user where id = ?;");
        stmt->setInteger(1, value);
        auto result = stmt->executeQuery();
        while (result->next()) {
            printf("id = %s, name = %s\n", result->getColumnByIndex(0), result->getColumnByIndex(1));
        }
    }
}