#pragma once

namespace sese::db {
    enum class MetadataType {
        TEXT,
        BOOLEAN,
        CHAR,
        SHORT,
        INTEGER,
        LONG,
        FLOAT,
        DOUBLE,
        DATE,
        TIME,
        DATE_TIME,
        UNKNOWN
    };
}