#include "sese/config/CSVWriter.h"

const char *sese::CSVWriter::CRLF = "\r\n";

sese::CSVWriter::CSVWriter(sese::OutputStream *dest, char splitChar) noexcept {
    CSVWriter::dest = dest;
    CSVWriter::splitChar = splitChar;
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
    dest->write(CRLF, 2);
}