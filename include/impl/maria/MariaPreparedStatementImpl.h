#pragma once
#include <sese/db/PreparedStatement.h>
#include <maria/MariaStmtResultSetImpl.h>

namespace sese::db::impl {

    class SESE_DB_API MariaPreparedStatementImpl : public PreparedStatement {
    public:
        explicit MariaPreparedStatementImpl(MYSQL_STMT *stmt, size_t count) noexcept;
        ~MariaPreparedStatementImpl() noexcept override;

        ResultSet::Ptr executeQuery() noexcept override;
        int64_t executeUpdate() noexcept override;

        bool setDouble(uint32_t index, double &value) noexcept override;  //设置双精度
        bool setFloat(uint32_t index, float &value) noexcept override;  //设置浮点型
        bool setInteger(uint32_t index, int32_t &value) noexcept override;  //设置int
        bool setLong(uint32_t index, int64_t &value) noexcept override;   //设置长整型
        bool setText(uint32_t index, const char *value) noexcept override;  //设置文本类型
        bool setNull(uint32_t index) noexcept override;     //设置空

        static bool mallocBindStruct(MYSQL_RES *res, MYSQL_BIND **bind) noexcept;
        static void freeBindStruct(MYSQL_BIND *bind, size_t count) noexcept;

    protected:
        MYSQL_STMT *stmt;   //
        size_t count = 0;
        MYSQL_BIND *param;   //发送给数据库的参数
    };

}// namespace sese::db