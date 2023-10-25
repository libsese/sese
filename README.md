# sese-db

This project is a part of [sese-core](https://github.com/libsese/sese-core) for now.

## support database

- ✅SQLite
- ✅MySQL
- ✅MariaDB
- ✅PostgreSQL

## example:

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

## connection string

- SQLite

  > "${path to database file}"

- MySQL、MariaDB、PostgreSQL

  > "host={ host };port={ port };user={ user };pwd={ password };db={ database name || schema name };"