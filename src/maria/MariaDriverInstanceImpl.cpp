#include <maria/MariaDriverInstanceImpl.h>
#include <maria/MariaResultSetImpl.h>

using namespace sese::db;

impl::MariaDriverInstanceImpl::MariaDriverInstanceImpl(MYSQL *conn) noexcept {
    this->conn = conn;
}

impl::MariaDriverInstanceImpl::~MariaDriverInstanceImpl() noexcept {
    mysql_close(conn);
}

ResultSet::Ptr impl::MariaDriverInstanceImpl::executeQuery(const char *sql) noexcept {
    if (0 != mysql_query(conn, sql)) {
        return nullptr;
    }
    MYSQL_RES *result = mysql_store_result(conn);
    return std::make_unique<MariaResultSetImpl>(result);
}

int64_t impl::MariaDriverInstanceImpl::executeUpdate(const char *sql) noexcept {
    if (0 != mysql_query(conn, sql)) {
        return -1;
    }
    return (int64_t) mysql_affected_rows(conn);
}

PreparedStatement::Ptr impl::MariaDriverInstanceImpl::createStatement(const char *sql) noexcept {
    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (stmt == nullptr) return nullptr;

    size_t len = strlen(sql);
    if (mysql_stmt_prepare(stmt, sql, (unsigned long) len)) return nullptr;

    // 此处需要手动计算 '?' 个数
    size_t count = 0;
    for (size_t i = 0; i < len; ++i) {
        if (sql[i] == '?') count++;
    }

    // 获取结果集元数据，对于非 DQL 而言这可能是 nullptr，不应作为错误处理
    auto meta = mysql_stmt_result_metadata(stmt);

    return std::make_unique<impl::MariaPreparedStatementImpl>(stmt, meta, count);
}

int impl::MariaDriverInstanceImpl::getLastError() const noexcept {
    return (int) mysql_errno(conn);
}

const char *impl::MariaDriverInstanceImpl::getLastErrorMessage() const noexcept {
    return mysql_error(conn);
}

bool impl::MariaDriverInstanceImpl::setAutoCommit(bool enable) noexcept {
    int temp;
    if (enable) {
        temp = (unsigned char) mysql_autocommit(conn, 1);
    } else {
        temp = (unsigned char) mysql_autocommit(conn, 0);
    }
    return temp == 0;
}

bool impl::MariaDriverInstanceImpl::getAutoCommit(bool &status) noexcept {
    auto rt = executeQuery("show variables like 'autocommit';");
    if (rt) {
        while (rt->next()) {
            status = strcmp(rt->getString(1).data(), "ON") == 0;
        }
        return true;
    } else {
        return false;
    }
}

bool impl::MariaDriverInstanceImpl::commit() noexcept {
    int comm = (unsigned char) mysql_commit(conn);
    return comm == 0;
}

bool impl::MariaDriverInstanceImpl::rollback() noexcept {
    int back = (unsigned char) mysql_rollback(conn);
    return back == 0;
}

bool impl::MariaDriverInstanceImpl::getInsertId(int64_t &id) const noexcept {
    id = (int64_t) mysql_insert_id(conn);
    if (id) {
        return true;
    } else {
        return false;
    }
}

bool impl::MariaDriverInstanceImpl::begin() noexcept {
    return mysql_query(conn, "BEGIN;") == 0;
}