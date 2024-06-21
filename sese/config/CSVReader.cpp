#include "sese/config/CSVReader.h"
#include "sese/text/StringBuilder.h"

sese::CSVReader::CSVReader(InputStream *source, char split_char) noexcept {
    CSVReader::source = source;
    CSVReader::splitChar = split_char;
}

sese::CSVReader::Row sese::CSVReader::read() noexcept {
    Row row;
    text::StringBuilder builder;

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
                row.emplace_back(builder.toString());
                builder.clear();
            } else if (ch == '\r') {
                // 换行 - \r\n
                if (source->read(&ch, 1) != 0) {
                    row.emplace_back(builder.toString());
                    builder.clear();
                    return row;
                }
            } else if (ch == '\n') {
                // 换行 - \n
                row.emplace_back(builder.toString());
                builder.clear();
                return row;
            } else {
                builder << ch;
            }
        }
    }
    if (!builder.empty() || ch == ',') {
        row.emplace_back(builder.toString());
        builder.clear();
    }
    return row;
}