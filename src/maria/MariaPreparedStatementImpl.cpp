#include <maria/MariaPreparedStatementImpl.h>

sese::db::impl::MariaPreparedStatementImpl::MariaPreparedStatementImpl(
        MYSQL_STMT *stmt,
        MYSQL_RES *meta,
        size_t count
) noexcept {
    this->stmt = stmt;
    this->meta = meta;
    this->count = count;
    this->param = (MYSQL_BIND *) malloc(sizeof(MYSQL_BIND) * count);
    memset(this->param, 0, sizeof(MYSQL_BIND) * count);
}

sese::db::impl::MariaPreparedStatementImpl::~MariaPreparedStatementImpl() noexcept {
    mysql_stmt_close(stmt);
    free(param);
    if (meta) {
        mysql_free_result(meta);
    }
}

//为可能的数据类型分配内存
bool sese::db::impl::MariaPreparedStatementImpl::mallocBindStruct(MYSQL_RES *res, MYSQL_BIND **bind) noexcept {
    *bind = (MYSQL_BIND *) malloc(sizeof(MYSQL_BIND) * res->field_count);
    memset(*bind, 0, sizeof(MYSQL_BIND) * res->field_count);
    for (unsigned int i = 0; i < res->field_count; i++) {
        // 非定长数据
        void **p = &((*bind)[i].buffer);
        switch (res->fields[i].type) {
            case MYSQL_TYPE_BLOB:
            case MYSQL_TYPE_TINY_BLOB:
            case MYSQL_TYPE_STRING:
            case MYSQL_TYPE_VARCHAR:
            case MYSQL_TYPE_VAR_STRING:
                *p = malloc(res->fields[i].length);
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
            {
                *p = malloc(sizeof(MYSQL_TIME));
                memset(*p, 0, sizeof(MYSQL_TIME));
                break;
            }
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
    free(bind);
}

sese::db::ResultSet::Ptr sese::db::impl::MariaPreparedStatementImpl::executeQuery() noexcept {
    if (!meta) return nullptr;

    if (mysql_stmt_bind_param(stmt, this->param)) {
        return nullptr;
    }
    MYSQL_BIND *result;
    if (!mallocBindStruct(meta, &result)) {
        goto freeResult;
    }
    if (mysql_stmt_bind_result(stmt, result)) {
        goto freeResult;
    }
    if (mysql_stmt_execute(stmt)) {
        goto freeResult;
    }
    if (mysql_stmt_store_result(stmt)) {
        goto freeResult;
    }
    return std::make_unique<impl::MariaStmtResultSet>(stmt, result, meta->field_count);

freeResult:
    freeBindStruct(result, meta->field_count);
    return nullptr;
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
    this->param[index - 1].buffer_length = (unsigned long) strlen(value);
    return true;
}

bool sese::db::impl::MariaPreparedStatementImpl::setNull(uint32_t index) noexcept {
    if (index - 1 >= count) return false;
    this->param[index - 1].buffer_type = MYSQL_TYPE_NULL;
    return true;
}

bool sese::db::impl::MariaPreparedStatementImpl::setDateTime(uint32_t index, const sese::DateTime &value) noexcept {
    if (index - 1 >= count) return false;
    if (!this->isDatetime[index - 1]) {
        this->param[index - 1].buffer = malloc(sizeof(MYSQL_TIME));
        this->isDatetime[index - 1] = true;
        SESE_DEBUG("alloc buffer addr: %p", this->param[index - 1].buffer);
    } else {
        SESE_DEBUG("use buffer addr: %p", this->param[index - 1].buffer);
    }
    memset(this->param[index - 1].buffer, 0 , sizeof(MYSQL_TIME));
    auto time = static_cast<MYSQL_TIME *>(this->param[index - 1].buffer);
    time->year = value.getYears();
    time->month = value.getMonths();
    time->day = value.getDays();
    time->hour = value.getHours();
    time->minute = value.getMinutes();
    time->second = value.getSeconds();
    time->time_type = MYSQL_TIMESTAMP_DATETIME;

    this->param[index - 1].buffer_type = MYSQL_TYPE_DATETIME;
    this->param[index - 1].buffer_length = sizeof(MYSQL_TIME);
    return true;
}

int sese::db::impl::MariaPreparedStatementImpl::getLastError() const noexcept {
    return (int) mysql_stmt_errno(stmt);
}

const char *sese::db::impl::MariaPreparedStatementImpl::getLastErrorMessage() const noexcept {
    return mysql_stmt_error(stmt);
}

bool sese::db::impl::MariaPreparedStatementImpl::getColumnType(uint32_t index, sese::db::MetadataType &type) noexcept {
    if ((nullptr == meta) || (meta->field_count <= index)) {
        return false;
    }

    switch (meta->fields[index].type) {
        case MYSQL_TYPE_STRING:
        case MYSQL_TYPE_VARCHAR:
        case MYSQL_TYPE_VAR_STRING:
            type = MetadataType::Text;
            break;
        case MYSQL_TYPE_TINY:
            type = MetadataType::Char;
            break;
        case MYSQL_TYPE_SHORT:
            type = MetadataType::Short;
            break;
        case MYSQL_TYPE_LONG:
            type = MetadataType::Integer;
            break;
        case MYSQL_TYPE_LONGLONG:
            type = MetadataType::Long;
            break;
        case MYSQL_TYPE_FLOAT:
            type = MetadataType::Float;
            break;
        case MYSQL_TYPE_DOUBLE:
            type = MetadataType::Double;
            break;
        case MYSQL_TYPE_TIME:
            type = MetadataType::Time;
            break;
        case MYSQL_TYPE_DATE:
            type = MetadataType::Date;
            break;
        case MYSQL_TYPE_DATETIME:
            type = MetadataType::DateTime;
            break;
        default:
            type = MetadataType::Unknown;
            break;
    }
    return true;
}

int64_t sese::db::impl::MariaPreparedStatementImpl::getColumnSize(uint32_t index) noexcept {
    // 当类型为二进制数据或者字符串时，该值才具有参考意义
    // 注意单位是字节，字符串需要考虑字符集问题
    if ((nullptr == meta) || (meta->field_count <= index)) {
        return -1;
    }
    return static_cast<int64_t>(meta->fields[index].length);
}
