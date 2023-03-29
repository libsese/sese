#pragma once

#include "sese/util/OutputStream.h"

#include <vector>
#include <string>

namespace sese {
    class API CSVWriter {
    public:
        using Row = std::vector<std::string>;

        static const char *CRLF;

        explicit CSVWriter(OutputStream *dest, char spiltChar = ',') noexcept;

        void write(const Row &row) noexcept;

    protected:
        char splitChar;
        OutputStream *dest = nullptr;
    };
}