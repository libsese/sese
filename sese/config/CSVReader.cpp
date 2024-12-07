// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
        // Is in string mode?
        if (quot % 2 == 1) {
            if (ch != '\"') {
                builder << ch;
            } else {
                // exit string mode
                quot += 1;
            }
        } else {
            if (ch == '\"') {
                // enter string mode
                quot += 1;
            } else if (ch == CSVReader::splitChar) {
                // Split
                row.emplace_back(builder.toString());
                builder.clear();
            } else if (ch == '\r') {
                // Split lines - \r\n
                if (source->read(&ch, 1) != 0) {
                    row.emplace_back(builder.toString());
                    builder.clear();
                    return row;
                }
            } else if (ch == '\n') {
                // Split lines - \n
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