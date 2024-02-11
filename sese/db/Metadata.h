#pragma once

namespace sese::db {
    enum class MetadataType {
        Text,
        Boolean,
        Char,
        Short,
        Integer,
        Long,
        Float,
        Double,
        Date,
        Time,
        DateTime,
        Unknown
    };
}