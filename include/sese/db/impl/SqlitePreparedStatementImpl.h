#include <sese/db/PreparedStatement.h>
#include <sese/db/impl/SqliteStmtResultSetImpl.h>

namespace sese::db::impl {

    class SESE_DB_API SqlitePreparedStatementImpl : public PreparedStatement {
    public:
        explicit SqlitePreparedStatementImpl(sqlite3_stmt *stmt) noexcept;
        ~SqlitePreparedStatementImpl() noexcept override;

        ResultSet::Ptr executeQuery() noexcept override;
        int64_t executeUpdate() noexcept override;
        int64_t setDouble(uint32_t index, double value) noexcept override;
        int64_t setInteger(uint32_t index, int64_t value) noexcept override;
        int64_t setText(uint32_t index, const char *value) noexcept override;
        int64_t setNull(uint32_t index) noexcept override;

    private:
        sqlite3_stmt *stmt;
    };
}