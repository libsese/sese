#pragma once

#include "sese/util/InputStream.h"

#include <vector>
#include <string>

namespace sese {

    class API CSVReader {
    public:
        using Row = std::vector<std::string>;

        explicit CSVReader(InputStream *source, char splitChar = ',') noexcept;

        Row read() noexcept;

    protected:
        char splitChar;
        InputStream *source = nullptr;
    };
}
