# sese-db
本项目为 [sese](https://github.com/shiinasama/sese) 的子项目（待整合）

目前支持的数据库有 Sqlite、MariaDB、Mysql。

## example

```c++
#include <sese/db/DriverManager.h>

using sese::db::DatabaseType;
using sese::db::DriverInstance;
using sese::db::DriverManager;
using sese::db::ResultSet;

int main () {
    auto instance = DriverInstance::getInstance(
        DatabaseType::MySql, 
        "host=127.0.0.1;user=root;pwd=xxxx;db=db_test;port=3306;"
    );
    if (!instance) {
        // failed to get the driver instance
    }
    
    auto result = instance->executeQuery("select * from tb_user;");
    if (!result) {
        // failed to execute query
    }
    
    while (result->next()) {
        printf("id = %d, name = %s\n", result->getInteger(0), result->getString(1).data());
    }
    
    return 0;
}
```

如上所示 DriverInstance::getInstance 的第二个参数为数据库连接字符串。使用不同的数据库驱动时有不同的写法。

其中 MySql 和 MariaDB 的连接字符串形式与上面一致。Sqlite 的连接字符串为文件的相对或绝对位置。
