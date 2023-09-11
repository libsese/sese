#include "sese/config/CSVReader.h"

#include <sstream>

sese::CSVReader::CSVReader(InputStream *source, char splitChar) noexcept {
    CSVReader::source = source;
    CSVReader::splitChar = splitChar;
}

sese::CSVReader::Row sese::CSVReader::read() noexcept {
    Row row;
    std::stringstream builder;

    char ch = 0;
    size_t quot = 0;
    while ((source->read(&ch, 1)) != 0) {
        // 是否处于字符串内部
        if (quot % 2 == 1) {
            if (ch != '\"') {
                builder << ch;
            } else {
                // 退出字符串
                quot += 1;
            }
        } else {
            if (ch == '\"') {
                // 进入字符串
                quot += 1;
            } else if (ch == CSVReader::splitChar) {
                // 切割元素
                row.emplace_back(builder.str());
                builder.str("");
            } else if (ch == '\r') {
                // 换行 - \r\n
                if (source->read(&ch, 1) != 0) {
                    row.emplace_back(builder.str());
                    builder.str("");
                    return row;
                }
            } else if (ch == '\n') {
                // 换行 - \n
                row.emplace_back(builder.str());
                builder.str("");
                return row;
            } else {
                builder << ch;
            }
        }
    }
    if (builder.rdbuf()->in_avail() != 0 || ch == ',') {
        row.emplace_back(builder.str());
        builder.str("");
    }
    return row;
}