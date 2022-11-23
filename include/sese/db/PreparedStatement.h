#pragma once
#include <sese/db/ResultSet.h>

namespace sese::db {

    class SESE_DB_API PreparedStatement {
    public:
        using Ptr = std::unique_ptr<PreparedStatement>;

        virtual ~PreparedStatement() noexcept = default;

        virtual ResultSet::Ptr executeQuery() noexcept = 0;
        virtual int64_t executeUpdate() noexcept = 0;

        virtual int64_t setDouble(uint32_t index, double value) noexcept = 0;
        virtual int64_t setInteger(uint32_t index, int64_t value) noexcept = 0;
        virtual int64_t setText(uint32_t index, const char *value) noexcept = 0;
        virtual int64_t setNull(uint32_t index) noexcept = 0;
    };
}// namespace sese::db