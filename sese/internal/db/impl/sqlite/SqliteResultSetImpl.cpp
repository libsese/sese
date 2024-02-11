#include <sese/internal/db/impl/sqlite/SqliteResultSetImpl.h>
#include <sese/text/DateTimeParser.h>

using namespace sese::db;

impl::SqliteResultSetImpl::SqliteResultSetImpl(char **table, size_t r, size_t c, char *error) noexcept {
    rows = r;
    columns = c;
    this->table = table;
    this->error = error;
}

impl::SqliteResultSetImpl::~SqliteResultSetImpl() noexcept {
    sqlite3_free_table(table);
    if (this->error) sqlite3_free(error);
}

void impl::SqliteResultSetImpl::reset() noexcept {
    current = 0;
}

bool impl::SqliteResultSetImpl::next() noexcept {
    if (current + 1 > rows) {
        return false;
    } else {
        current += 1;
        return true;
    }
}

size_t impl::SqliteResultSetImpl::getColumns() const noexcept {
    return columns;
}

int32_t impl::SqliteResultSetImpl::getInteger(size_t index) const noexcept {
    char *p = table[(columns) *current + index];
    char *end;
    return static_cast<int32_t>(std::strtol(p, &end, 10));
}

std::string_view impl::SqliteResultSetImpl::getString(size_t index) const noexcept {
    return {table[(columns) *current + index]};
}

double impl::SqliteResultSetImpl::getDouble(size_t index) const noexcept {
    char *p = table[(columns) *current + index];
    char *end;
    return std::strtod(p, &end);
}

float impl::SqliteResultSetImpl::getFloat(size_t index) const noexcept {
    char *p = table[(columns) *current + index];
    char *end;
    return std::strtof(p, &end);
}

int64_t impl::SqliteResultSetImpl::getLong(size_t index) const noexcept {
    char *p = table[(columns) *current + index];
    char *end;
    return std::strtoll(p, &end, 10);
}
std::optional<sese::DateTime> impl::SqliteResultSetImpl::getDateTime(size_t index) const noexcept {
    std::optional<sese::DateTime> rt = text::DateTimeParser::parse("yyyy-MM-dd HH:mm:ss", table[(columns) *current + index]);
    return rt;
}
bool impl::SqliteResultSetImpl::isNull(size_t index) const noexcept {
    if (table[(columns) *current + index] == nullptr) return true;
    return false;
}
