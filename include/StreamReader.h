#pragma once
#include "Stream.h"
#include "ByteBuilder.h"
#include <string>

namespace sese {
    class API StreamReader {
    public:
        explicit StreamReader(Stream::Ptr source);

        bool read(char &ch);
        std::string readLine();

    private:
        int64_t preRead();

    private:
        Stream::Ptr sourceStream;
        ByteBuilder::Ptr bufferStream;
    };
}// namespace sese