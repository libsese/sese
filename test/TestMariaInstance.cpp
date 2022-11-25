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

    {
        auto count = instance->executeUpdate("update tb_user set name = 'shiina_kaoru' where id = 1;");
        printf("update rows: %d\n", (int) count);

        auto result = instance->executeQuery("select * from tb_user;");
        while (result->next()) {
            printf("id = %d, name = %s\n", result->getInteger(0), result->getString(1).data());
        }
    }

    {
        int64_t value = 1;
        auto stmt = instance->createStatement("select * from tb_user where id = ?;");
        stmt->setLong(1, value);
        auto result = stmt->executeQuery();
        while (result->next()) {
            printf("id = %d, name = %s\n",(int) result->getInteger(0), result->getString(1).data());
        }
    }

    {
        int64_t id = 9;
        const char *name = "sese-db";
        auto stmt1 = instance->createStatement("update tb_user set name = ? where id = ?");
        stmt1->setText(1, name);
        stmt1->setLong(2, id);
        auto count = stmt1->executeUpdate();
        printf("stmt update rows: %d\n", (int) count);
    }
}