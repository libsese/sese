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

#include "sese/config/CSVWriter.h"

const char *sese::CSVWriter::crlf = "\r\n";
const char *sese::CSVWriter::lf = "\n";

sese::CSVWriter::CSVWriter(OutputStream *dest, char split_char, bool crlf) noexcept {
    CSVWriter::dest = dest;
    CSVWriter::splitChar = split_char;
    CSVWriter::isCRLF = crlf;
}

void sese::CSVWriter::write(const sese::CSVWriter::Row &row) noexcept {
    bool first = true;
    for (const auto &col: row) {
        if (first) {
            first = false;
            dest->write(col.c_str(), col.size());
        } else {
            dest->write(&splitChar, 1);
            dest->write(col.c_str(), col.size());
        }
    }
    if (isCRLF) {
        dest->write(crlf, 2);
    } else {
        dest->write(lf, 1);
    }
}