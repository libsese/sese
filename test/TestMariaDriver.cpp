#include <mysql.h>
#include <cstdio>

int main() {
    MYSQL *conn = mysql_init(nullptr);

    mysql_real_connect(
            conn,
            "127.0.0.1",
            "root",
            "2001",
            "db_test",
            3306,
            nullptr,
            0
    );

    auto rt = mysql_query(conn, "select user.name from tb_user user where id = 1;");
    auto res = mysql_store_result(conn);
    if (rt == 0) {
        auto row = mysql_fetch_row(res);
        puts(row[0]);
    } else {
        printf("failed to query");
    }

    mysql_free_result(res);
    mysql_close(conn);
    return 0;
}