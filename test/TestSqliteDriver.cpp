#include <sqlite3.h>
#include <cstdio>

int main () {
    int rows;
    int columns;
    char *error;
    char **table;

    sqlite3 *conn;
    sqlite3_open_v2("db_test.db", &conn, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE , nullptr);
    sqlite3_get_table(conn, "select user.name from db_user user where id = 1;", &table, &rows, &columns, &error);
    puts(table[1]);

    sqlite3_free_table(table);
    sqlite3_close_v2(conn);
}