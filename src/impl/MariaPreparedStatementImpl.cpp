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

bool sese::db::impl::MariaPreparedStatementImpl::mallocBindStruct(MYSQL_RES *res, MYSQL_BIND **bind) noexcept {
    *bind = (MYSQL_BIND *) malloc(sizeof(MYSQL_BIND) * res->field_count);
    memset(*bind, 0, sizeof(MYSQL_BIND) * res->field_count);
    for (auto i = 0; i < res->field_count; i++) {
        // 非定长数据
        void **p = &((*bind)[i].buffer);
        switch (res->fields[i].type) {
            case MYSQL_TYPE_BLOB:
            case MYSQL_TYPE_TINY_BLOB:
            case MYSQL_TYPE_STRING:
            case MYSQL_TYPE_VARCHAR:
            case MYSQL_TYPE_VAR_STRING:
                *p = malloc(res->fields[i].length / 4);
                (*bind)[i].buffer_length = res->fields[i].length;
                break;
            case MYSQL_TYPE_TINY:
                *p = malloc(1);
                break;
            case MYSQL_TYPE_SHORT:
                *p = malloc(2);
                break;
            case MYSQL_TYPE_LONG:
                *p = malloc(4);
                break;
            case MYSQL_TYPE_LONGLONG:
                *p = malloc(8);
                break;
            case MYSQL_TYPE_FLOAT:
                *p = malloc(4);
                break;
            case MYSQL_TYPE_DOUBLE:
                *p = malloc(8);
                break;
            case MYSQL_TYPE_TIME:
            case MYSQL_TYPE_DATE:
            case MYSQL_TYPE_DATETIME:
                *p = malloc(sizeof(MYSQL_TIME));
                break;
            default:
                return false;
        }
        (*bind)[i].buffer_type = res->fields[i].type;
    }
    return true;
}

void sese::db::impl::MariaPreparedStatementImpl::freeBindStruct(MYSQL_BIND *bind, size_t count) noexcept {
    for (auto i = 0; i < count; ++i) {
        if (bind[i].buffer != nullptr) free(bind[i].buffer);
    }
}

sese::db::ResultSet::Ptr sese::db::impl::MariaPreparedStatementImpl::executeQuery() noexcept {
    if (mysql_stmt_bind_param(stmt, this->param)) {
        return nullptr;
    }
    // 通过 res 信息构建 MYSQL_BIND
    auto res = mysql_stmt_result_metadata(stmt);
    MYSQL_BIND *result;
    if(!mallocBindStruct(res, &result)) {
        freeBindStruct(result, res->field_count);
        return nullptr;
    }

    if (mysql_stmt_bind_result(stmt, result)) {
        return nullptr;
    }
    if (mysql_stmt_execute(stmt)) {
        return nullptr;
    }
    if (mysql_stmt_store_result(stmt)) {
        return nullptr;
    }
    return std::make_unique<impl::MariaStmtResultSet>(stmt, result, res->field_count);
}

int64_t sese::db::impl::MariaPreparedStatementImpl::executeUpdate() noexcept {
    if (mysql_stmt_bind_param(stmt, this->param)) {
        return -1;
    }

    if (mysql_stmt_execute(stmt)) {
        return -1;
    }
    return (int64_t) mysql_stmt_affected_rows(stmt);
}

bool sese::db::impl::MariaPreparedStatementImpl::setDouble(uint32_t index, double &value) noexcept {
    if (index - 1 >= count) return false;
    this->param[index - 1].buffer_type = MYSQL_TYPE_DOUBLE;
    this->param[index - 1].buffer = &value;
    return true;
}

bool sese::db::impl::MariaPreparedStatementImpl::setFloat(uint32_t index, float &value) noexcept {
    if (index - 1 >= count) return false;
    this->param[index - 1].buffer_type = MYSQL_TYPE_FLOAT;
    this->param[index - 1].buffer = &value;
    return true;
}

bool sese::db::impl::MariaPreparedStatementImpl::setInteger(uint32_t index, int32_t &value) noexcept {
    if (index - 1 >= count) return false;
    this->param[index - 1].buffer_type = MYSQL_TYPE_LONG;
    this->param[index - 1].buffer = &value;
    return true;
}

bool sese::db::impl::MariaPreparedStatementImpl::setLong(uint32_t index, int64_t &value) noexcept {
    if (index - 1 >= count) return false;
    this->param[index - 1].buffer_type = MYSQL_TYPE_LONGLONG;
    this->param[index - 1].buffer = &value;
    return true;
}

bool sese::db::impl::MariaPreparedStatementImpl::setText(uint32_t index, const char *value) noexcept {
    if (index - 1 >= count) return false;
    this->param[index - 1].buffer_type = MYSQL_TYPE_VAR_STRING;
    this->param[index - 1].buffer = (void *) value;
    this->param[index - 1].buffer_length = strlen(value);
    return true;
}

bool sese::db::impl::MariaPreparedStatementImpl::setNull(uint32_t index) noexcept {
    if (index - 1 >= count) return false;
    this->param[index - 1].buffer_type = MYSQL_TYPE_NULL;
    return true;
}
