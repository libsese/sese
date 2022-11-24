#include <sese/db/impl/MariaPreparedStatementImpl.h>

sese::db::impl::MariaPreparedStatementImpl::MariaPreparedStatementImpl(MYSQL_STMT *stmt, size_t count) noexcept {
    this->stmt = stmt;
    this->count = count;
    this->param = (MYSQL_BIND *) malloc(sizeof(MYSQL_BIND) * count);
    memset(this->param, 0, sizeof(MYSQL_BIND) * count);
}

sese::db::impl::MariaPreparedStatementImpl::~MariaPreparedStatementImpl() noexcept {
    mysql_stmt_close(stmt);
    free(param);
}

sese::db::ResultSet::Ptr sese::db::impl::MariaPreparedStatementImpl::executeQuery() noexcept {
    // if(mysql_stmt_bind_param(stmt, this->param)) {
    //     return nullptr;
    // }
    // if(mysql_stmt_execute(stmt)) {
    //     return nullptr;
    // }

    // 通过 res 信息构建 MYSQL_BIND
    // auto res = mysql_stmt_result_metadata(stmt);
    // auto result = (MYSQL_BIND *) malloc(sizeof(MYSQL_BIND) * res->field_count);
    // memset(result, 0, sizeof(MYSQL_BIND) * res->field_count);
    // mysql_free_result(res);

    // if(mysql_stmt_bind_result(stmt, result)) {
    //     return nullptr;
    // }
    // if(mysql_stmt_store_result(stmt)){
    //     return nullptr;
    // }

    // return std::make_unique<impl::MariaStmtResultSet>(stmt);

    return nullptr;
}

int64_t sese::db::impl::MariaPreparedStatementImpl::executeUpdate() noexcept {
    return 0;
}

int64_t sese::db::impl::MariaPreparedStatementImpl::setDouble(uint32_t index, double &value) noexcept {
    this->param[index - 1].buffer_type = MYSQL_TYPE_DOUBLE;
    this->param[index - 1].buffer = &value;
    return 0;
}

int64_t sese::db::impl::MariaPreparedStatementImpl::setInteger(uint32_t index, int64_t &value) noexcept {
    this->param[index - 1].buffer_type = MYSQL_TYPE_LONG;
    this->param[index - 1].buffer = &value;
    return 0;
}

int64_t sese::db::impl::MariaPreparedStatementImpl::setText(uint32_t index, const char *value) noexcept {
    this->param[index - 1].buffer_type = MYSQL_TYPE_STRING;
    this->param[index - 1].buffer = (void *) value;
    this->param[index - 1].buffer_length = strlen(value);
    return 0;
}

int64_t sese::db::impl::MariaPreparedStatementImpl::setNull(uint32_t index) noexcept {
    this->param[index - 1].buffer_type = MYSQL_TYPE_NULL;
    return 0;
}
