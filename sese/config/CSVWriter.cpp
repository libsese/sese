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